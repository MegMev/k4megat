#include "Analysis/EdmUtils.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Math/SMatrix.h"
#include "TFile.h"
#include "TTree.h"
#include "edm4hep/SimTrackerHitCollection.h"
#include "edm4hep/TrackerHitCollection.h"
#include "k4FWCore/DataHandle.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <mlpack/core.hpp>
#include <mlpack/methods/kmeans/kmeans.hpp>
#include <numeric>
#include <vector>

class ClusteringInterpolationAlg : public GaudiAlgorithm {
public:
  using SVector3  = ROOT::Math::SVector<double, 3>;
  using SVector4  = ROOT::Math::SVector<double, 4>;
  using SMatrix33 = ROOT::Math::SMatrix<double, 3, 3>;
  using RVec3d    = ROOT::VecOps::RVec<edm4hep::Vector3d>;
  using RVecF     = ROOT::RVecF;

  explicit ClusteringInterpolationAlg( const std::string& aName, ISvcLocator* aSvcLoc )
      : GaudiAlgorithm( aName, aSvcLoc ) {
    // 声明输入和输出
    declareProperty( "inHits", m_inHits, "Segmented Hit Collection of Tpc" );
    declareProperty( "outHits", m_outHits, "Smeared Electron Hit Collection" );
  }

  StatusCode initialize() final {
    // 调用基类的initialize方法
    if ( GaudiAlgorithm::initialize().isFailure() ) return StatusCode::FAILURE;

    // // 创建ROOT文件和TTree，用于保存处理后的数据
    // m_rootFile = new TFile("/path/to/output_file.root", "RECREATE");
    // m_kfTree = new TTree("KFData", "Data for KF");

    // // 定义TTree的分支
    // m_kfTree->Branch("id", &m_eventID);
    // m_kfTree->Branch("fx", &m_enhanceDataFx);
    // m_kfTree->Branch("fy", &m_enhanceDataFy);
    // m_kfTree->Branch("fz", &m_enhanceDataFz);

    return StatusCode::SUCCESS;
  }

  StatusCode execute() final {
    // 读取输入数据集合
    const auto in_hits  = m_inHits.get();
    auto       out_hits = m_outHits.createAndPut();

    // 提取输入数据中的x, y, z和电荷值
    // std::vector<float> ori_fx, ori_fy, ori_fz, ori_fe;
    // for (const auto& hit : *in_hits) {
    //   ori_fx.push_back(hit.getPosition().x);
    //   ori_fy.push_back(hit.getPosition().y);
    //   ori_fz.push_back(hit.getPosition().z);
    //   ori_fe.push_back(hit.getEDep());
    // }

    auto orig_pos = in_hits->position();
    auto ori_fx   = megat::utility::get_x( RVec3d( orig_pos ) );
    auto ori_fy   = megat::utility::get_y( RVec3d( orig_pos ) );
    auto ori_fz   = 255 - RVecF( in_hits->time() ) * 0.033;
    // auto ori_fz = megat::utility::get_z(RVec3d(orig_pos));
    // auto ori_fe = RVecF(in_hits->EDep());
    auto ori_fe = RVecF( in_hits->eDep() );

    float e_energy = ROOT::VecOps::Sum( ori_fe );

    // 如果数据点数量小于4，直接返回
    if ( ori_fx.size() < 4 ) return StatusCode::SUCCESS;

    // 将数据转换为向量格式
    std::vector<SVector4> data;
    for ( size_t i = 0; i < ori_fx.size(); ++i ) {
      data.push_back( SVector4{ ori_fx[i], ori_fy[i], ori_fz[i], ori_fe[i] } );
    }

    // 执行聚类操作
    auto cluster_centers = performClustering( data );
    if ( cluster_centers.size() < 4 ) return StatusCode::SUCCESS;

    // 对聚类中心进行排序
    auto sorted_points = sortClusterCenters( cluster_centers );

    // 进行插值计算
    auto interpolated_points = interpolatePoints( sorted_points );

    // 计算电荷中心
    auto charge_center_points = calculateChargeCenters( data, interpolated_points );

    // 如果电荷中心点数量小于5，直接返回
    if ( charge_center_points.size() < 5 ) return StatusCode::SUCCESS;

    // 保存插值点
    // saveInterpolatedPoints(charge_center_points);
    // 保存插值点到TTree
    {
      m_enhanceDataFx.clear();
      m_enhanceDataFy.clear();
      m_enhanceDataFz.clear();

      int i = 0;
      for ( const auto& point : charge_center_points ) {
        m_enhanceDataFx.push_back( point[0] );
        m_enhanceDataFy.push_back( point[1] );
        m_enhanceDataFz.push_back( point[2] );
        auto new_hit = out_hits->create();
        new_hit.setPosition( { m_enhanceDataFx[i], m_enhanceDataFy[i], m_enhanceDataFz[i] } );
        new_hit.setEDep( e_energy / charge_center_points.size() );
        // SVector3 new_point(m_enhanceDataFx[i], m_enhanceDataFy[i], m_enhanceDataFz[i]);
        // out_hits->push_back( new_point );
        i++;
      }

      //    m_kfTree->Fill();
    }

    return StatusCode::SUCCESS;
  }

  StatusCode finalize() final {
    // // 写入并关闭ROOT文件
    // m_rootFile->Write();
    // m_rootFile->Close();
    // delete m_rootFile;
    return GaudiAlgorithm::finalize();
  }

private:
  // 计算两个向量的叉积
  SVector3 calculateVectorProduct( const SVector3& vector1, const SVector3& vector2 ) {
    return SVector3( vector1[1] * vector2[2] - vector1[2] * vector2[1],
                     vector1[2] * vector2[0] - vector1[0] * vector2[2],
                     vector1[0] * vector2[1] - vector1[1] * vector2[0] );
  }

  // 计算向量与z轴的夹角的余弦和正弦值
  std::pair<double, double> calculateCosSinWithZAxis( const SVector3& f_vector ) {
    SVector3 unit_f_vector = f_vector / ROOT::Math::Mag( f_vector );
    double   f_cos_theta   = ROOT::Math::Dot( unit_f_vector, v );
    double   f_sin_theta   = std::sqrt( 1 - f_cos_theta * f_cos_theta );
    return std::make_pair( f_cos_theta, f_sin_theta );
  }

  // 将向量旋转为平行于z轴
  SVector3 rotateVectorParallelZ( const SVector3& f_array, const SVector3& f_start_point,
                                  const SVector3& f_end_point ) {
    SVector3 f_vector      = f_end_point - f_start_point;
    SVector3 unit_f_vector = f_vector / ROOT::Math::Mag( f_vector );
    SVector3 n             = calculateVectorProduct( unit_f_vector, v );
    n                      = n / ROOT::Math::Mag( n );
    double    a[9]         = { 0, -n[2], n[1], n[2], 0., -n[0], -n[1], n[0], 0 };
    SMatrix33 N( a, 9 );
    SMatrix33 N2                    = N * N;
    auto [f_cos_theta, f_sin_theta] = calculateCosSinWithZAxis( unit_f_vector );
    SMatrix33 R                     = I + f_sin_theta * N + ( 1 - f_cos_theta ) * N2;
    return R * f_array;
  }

  // 计算电荷中心
  SVector3 calculateChargeCenter( const std::vector<SVector4>& charge_data ) {
    double   total_charge = 0;
    SVector3 charge_center( 0, 0, 0 );
    for ( const auto& hit : charge_data ) {
      total_charge += hit[3];
      charge_center[0] += hit[0] * hit[3];
      charge_center[1] += hit[1] * hit[3];
      charge_center[2] += hit[2] * hit[3];
    }
    charge_center /= total_charge;
    return charge_center;
  }

  // 使用mlpack库执行聚类操作
  std::vector<SVector3> performClustering( const std::vector<SVector4>& data ) {
    // 将数据转换为Armadillo矩阵
    arma::mat dataset( 3, data.size() );
    for ( size_t i = 0; i < data.size(); ++i ) {
      dataset( 0, i ) = data[i][0];
      dataset( 1, i ) = data[i][1];
      dataset( 2, i ) = data[i][2];
    }

    // 设置K值（聚类数量）
    const size_t      numClusters = 10;
    arma::Row<size_t> assignments;
    arma::mat         centroids;

    // 执行K-means聚类
    mlpack::kmeans::KMeans<> kmeans;
    kmeans.Cluster( dataset, numClusters, assignments, centroids );

    // 转换聚类中心到返回值
    std::vector<SVector3> cluster_centers( numClusters );
    for ( size_t i = 0; i < numClusters; ++i ) {
      cluster_centers[i][0] = centroids( 0, i );
      cluster_centers[i][1] = centroids( 1, i );
      cluster_centers[i][2] = centroids( 2, i );
    }

    return cluster_centers;
  }

  // 对聚类中心进行排序
  std::vector<SVector3> sortClusterCenters( const std::vector<SVector3>& cluster_centers ) {
    std::vector<SVector3> sorted_points;
    std::vector<bool>     visited( cluster_centers.size(), false ); // 记录每个聚类中心是否被访问
    sorted_points.push_back( cluster_centers[0] );                  // 从第一个聚类中心开始
    visited[0] = true;                                              // 标记第一个聚类中心已访问
    // 当排序后的点数量小于聚类中心总数时，继续循环
    while ( sorted_points.size() < cluster_centers.size() ) {
      auto   last_point     = sorted_points.back();               // 获取当前排序列表中的最后一个点
      double min_distance   = std::numeric_limits<double>::max(); // 初始化最小距离为无穷大
      int    next_point_idx = -1;                                 // 初始化下一个点的索引
      // 遍历所有聚类中心，找到最近的未访问过的聚类中心
      for ( size_t i = 0; i < cluster_centers.size(); ++i ) {
        if ( !visited[i] ) {
          double distance = ROOT::Math::Mag( last_point - cluster_centers[i] );
          if ( distance < min_distance ) {
            min_distance   = distance;
            next_point_idx = i;
          }
        }
      }
      // 如果找到了下一个点，将其添加到排序列表，并标记为已访问
      if ( next_point_idx != -1 ) {
        sorted_points.push_back( cluster_centers[next_point_idx] );
        visited[next_point_idx] = true;
      }
    }

    return sorted_points; // 返回排序后的聚类中心
  }

  // 进行插值计算
  std::vector<SVector4> interpolatePoints( const std::vector<SVector3>& sorted_points ) {
    std::vector<SVector4> interpolated_points;
    for ( size_t i = 0; i < sorted_points.size() - 1; ++i ) {
      SVector3 start_point = sorted_points[i];        // 当前点
      SVector3 end_point   = sorted_points[i + 1];    // 下一个点
      SVector3 step        = end_point - start_point; // 计算步长
      step /= ROOT::Math::Mag( step );                // 归一化步长向量

      double distance                = ROOT::Math::Mag( end_point - start_point );     // 计算两个点之间的距离
      int    num_interpolated_points = static_cast<int>( distance / INTER_PRECISION ); // 计算插值点的数量
      for ( int j = 0; j < num_interpolated_points; ++j ) {
        SVector3 interpolated_point = start_point + step * ( j * INTER_PRECISION );
        interpolated_points.push_back(
            SVector4( interpolated_point[0], interpolated_point[1], interpolated_point[2], 0 ) );
      }
      interpolated_points.push_back( SVector4( end_point[0], end_point[1], end_point[2], 0 ) ); // 添加终点到结果中
    }
    return interpolated_points;
  }

  // 计算电荷中心
  // std::vector<SVector3> calculateChargeCenters(const std::vector<SVector4>& data, const std::vector<SVector4>&
  // interpolated_points) {
  //   std::vector<SVector3> charge_center_points;
  //   for (size_t idx = 0; idx < interpolated_points.size() - 1; ++idx) {
  //       std::vector<SVector4> cluster_hits;
  //       SVector3 pre_start(interpolated_points[idx][0], interpolated_points[idx][1], interpolated_points[idx][2]);
  //       SVector3 pre_end(interpolated_points[idx + 1][0], interpolated_points[idx + 1][1], interpolated_points[idx +
  //       1][2]);

  //       for (const auto& hit : data) {
  //           SVector3 hit_position(hit[0], hit[1], hit[2]);
  //           if (ROOT::Math::Mag(hit_position - pre_start) < INTER_PRECISION) {
  //               cluster_hits.push_back(hit);
  //           }
  //       }

  //       if (!cluster_hits.empty()) {
  //           SVector3 charge_center = calculateChargeCenter(cluster_hits);
  //           charge_center_points.push_back(charge_center);
  //       }
  //   }
  //   return charge_center_points;
  // }

  std::vector<SVector3> calculateChargeCenters( const std::vector<SVector4>& data,
                                                const std::vector<SVector4>& interpolated_points ) {
    std::vector<SVector3> charge_center_points;
    for ( size_t idx = 0; idx < interpolated_points.size() - 1; ++idx ) {
      std::vector<SVector4> cluster_hits;
      SVector3 pre_start( interpolated_points[idx][0], interpolated_points[idx][1], interpolated_points[idx][2] );
      SVector3 pre_end( interpolated_points[idx + 1][0], interpolated_points[idx + 1][1],
                        interpolated_points[idx + 1][2] );

      for ( const auto& hit : data ) {
        SVector3 hit_position( hit[0], hit[1], hit[2] );
        if ( ROOT::Math::Mag( hit_position - pre_start ) < INTER_PRECISION ) { cluster_hits.push_back( hit ); }
      }

      if ( !cluster_hits.empty() ) {
        // 旋转簇中的电子数据，使其与Z轴平行
        std::vector<SVector3> transformed_electron_data;
        for ( const auto& hit : cluster_hits ) {
          transformed_electron_data.push_back(
              rotateVectorParallelZ( SVector3( hit[0], hit[1], hit[2] ), pre_start, pre_end ) );
        }
        SVector3 new_start = rotateVectorParallelZ( pre_start, pre_start, pre_end );
        SVector3 new_end   = rotateVectorParallelZ( pre_end, pre_start, pre_end );

        std::vector<SVector4> selected_points;
        for ( size_t i = 0; i < transformed_electron_data.size(); ++i ) {
          if ( new_end[2] < transformed_electron_data[i][2] && transformed_electron_data[i][2] <= new_start[2] ) {
            selected_points.push_back( cluster_hits[i] );
          }
        }

        if ( !selected_points.empty() ) {
          double total_charge = std::accumulate( selected_points.begin(), selected_points.end(), 0.0,
                                                 []( double sum, const SVector4& p ) { return sum + p[3]; } );
          if ( selected_points.size() > 5 && total_charge != 0 ) {
            SVector3 charge_center = calculateChargeCenter( selected_points );
            charge_center_points.push_back( charge_center );
          }
        }
      }
    }
    return charge_center_points;
  }

  /*   // 保存插值点到TTree
    void saveInterpolatedPoints(const std::vector<SVector3>& points) {
      m_enhanceDataFx.clear();
      m_enhanceDataFy.clear();
      m_enhanceDataFz.clear();

      auto out_hits = m_outHits.createAndPut();
      int i=0;
      for (const auto& point : points) {
        m_enhanceDataFx.push_back(point[0]);
        m_enhanceDataFy.push_back(point[1]);
        m_enhanceDataFz.push_back(point[2]);
        auto new_hit = out_hits->create();
        new_hit.setPosition( { m_enhanceDataFx[i], m_enhanceDataFy[i], m_enhanceDataFz[i]} );
        // SVector3 new_point(m_enhanceDataFx[i], m_enhanceDataFy[i], m_enhanceDataFz[i]);
        // out_hits->push_back( new_point );
        i++;
      }

  //    m_kfTree->Fill();
    } */

  // 数据Data Handle
  DataHandle<edm4hep::TrackerHitCollection> m_inHits{ "TpcHits", Gaudi::DataHandle::Reader, this };
  DataHandle<edm4hep::TrackerHitCollection> m_outHits{ "TpcCIHits", Gaudi::DataHandle::Writer, this };

  // 输出的ROOT文件和TTree
  //  TFile* m_rootFile{nullptr};
  //  TTree* m_kfTree{nullptr};

  // TTree分支变量
  int                 m_eventID;
  std::vector<double> m_enhanceDataFx, m_enhanceDataFy, m_enhanceDataFz;

  // 常量和成员变量
  const SVector3  v               = { 0, 0, -1 };
  const SMatrix33 I               = ROOT::Math::SMatrixIdentity();
  const int       INTER_PRECISION = 10;
};

// 声明算法组件
DECLARE_COMPONENT( ClusteringInterpolationAlg )
