#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/RndmGenerators.h"
#include "k4FWCore/DataHandle.h"

#include "Math/SMatrix.h"
#include "TRandom3.h"
#include "edm4hep/SimTrackerHitCollection.h"
#include "edm4hep/TrackerHitCollection.h"

#include "Analysis/EdmUtils.h"

/** @class KalmanFilterDemoAlg
 *
 *
 *  Input: SimTrackerHit from simulation
 *
 *  Output: Kalman-filtered SimTrackerHit
 *
 *  @author Yong Zhou
 */

class KalmanFilterDemoAlg : public GaudiAlgorithm {
public:
  using SMatrix66 = ROOT::Math::SMatrix<double, 6, 6>;
  using SMatrix36 = ROOT::Math::SMatrix<double, 3, 6>;
  using SMatrix63 = ROOT::Math::SMatrix<double, 6, 3>;
  using SMatrix33 = ROOT::Math::SMatrix<double, 3, 3>;
  using SVector6  = ROOT::Math::SVector<double, 6>;
  using SVector3  = ROOT::Math::SVector<double, 3>;
  using RVec3d    = ROOT::VecOps::RVec<edm4hep::Vector3d>;
  using RVecF     = ROOT::RVecF;

  explicit KalmanFilterDemoAlg( const std::string& aName, ISvcLocator* aSvcLoc ) : GaudiAlgorithm( aName, aSvcLoc ) {
    declareProperty( "inHits", m_inHits, "Segmented Hit Collection of Tpc" );
    //    declareProperty( "digiHits", m_digiHits, "Segmented Hit Collection of Tpc" );
    declareProperty( "outHits", m_outHits, "Smeared Electron Hit Collection" );
  }

  /* Pre-run callback */
  StatusCode initialize() final {
    /// mandatory
    if ( GaudiAlgorithm::initialize().isFailure() ) return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
  }

  /* Event-processing callback */
  StatusCode execute() final {
    /// get input & create output hit collections
    const auto in_hits = m_inHits.get();
    //    const auto digi_hits = m_digiHits.get();
    auto out_hits = m_outHits.createAndPut();

    /// ori_fx, ori_fy, ori_fz, ori_fe are all RVec
    auto orig_pos = in_hits->position();
    auto ori_fx   = megat::utility::get_x( RVec3d( orig_pos ) );
    auto ori_fy   = megat::utility::get_y( RVec3d( orig_pos ) );
    // auto ori_fz = 255-RVecF(in_hits->time())*0.033;
    auto ori_fz = megat::utility::get_z( RVec3d( orig_pos ) );
    // auto ori_fe = RVecF(in_hits->EDep());
    auto ori_fe = RVecF( in_hits->eDep() );

    /// filered output
    std::vector<float> KF_x;
    std::vector<float> KF_y;
    std::vector<float> KF_z;

    float e_energy       = ROOT::VecOps::Sum( ori_fe );
    int   num_of_oriHits = ori_fx.size();
    if ( num_of_oriHits <= 1 ) return StatusCode::SUCCESS;

    std::vector<float> dLx, dLy, dLz, dL;
    std::vector<float> speedx_group, speedy_group, speedz_group;
    float              dz = 1;

    //[todo] e_energy unit, the default is GeV in edm4hep? 多次散射角
    float thetarnd0 = 13600. / e_energy * 1e6 * std::sqrt( dz / 23520. );

    for ( int k = 0; k < ori_fx.size() - 1; k++ ) {
      float speedx, speedy, speedz = 0.;

      dLx.push_back( ori_fx[k + 1] - ori_fx[k] );
      dLy.push_back( ori_fy[k + 1] - ori_fy[k] );
      dLz.push_back( ori_fz[k + 1] - ori_fz[k] );
      dL.push_back( std::sqrt( std::pow( dLx[k], 2 ) + std::pow( dLy[k], 2 ) + std::pow( dLz[k], 2 ) ) );

      speedx = ( m_gaus.Gaus( dLx[k], thetarnd0 ) );
      speedy = ( m_gaus.Gaus( dLy[k], thetarnd0 ) );
      speedz = ( m_gaus.Gaus( dLz[k], thetarnd0 ) );

      speedx_group.push_back( speedx / std::sqrt( pow( speedx, 2 ) + std::pow( speedy, 2 ) + std::pow( speedz, 2 ) ) );
      speedy_group.push_back( speedy / std::sqrt( pow( speedx, 2 ) + std::pow( speedy, 2 ) + std::pow( speedz, 2 ) ) );
      speedz_group.push_back( speedz / std::sqrt( pow( speedx, 2 ) + std::pow( speedy, 2 ) + std::pow( speedz, 2 ) ) );
    }

    //参数准备
    double Speedx, Speedy, Speedz;

    //卡尔曼滤波点和平滑过程点
    double KFx[num_of_oriHits], KFy[num_of_oriHits], KFz[num_of_oriHits], KFvx[num_of_oriHits], KFvy[num_of_oriHits],
        KFvz[num_of_oriHits], SRx[num_of_oriHits], SRy[num_of_oriHits], SRz[num_of_oriHits], SRvx[num_of_oriHits],
        SRvy[num_of_oriHits], SRvz[num_of_oriHits], NUM[num_of_oriHits];

    SVector3  XMscoli;
    SVector6  Expectation; // 6D的预测vector
    SVector6* ExpectationRecord = new SVector6[num_of_oriHits];
    SVector6  Kalmanresult; //滤波
    SVector6  SmoothResult; //平滑

    SMatrix66 F;                 // 6*6的状态矩阵F
    SMatrix36 measureMatrix;     // 3*6的测量矩阵H
    measureMatrix( 0, 0 ) = 1.0; // H的初始值
    measureMatrix( 1, 1 ) = 1.0;
    measureMatrix( 2, 2 ) = 1.0;
    SMatrix66 I; // 6*6的单位阵
    I( 0, 0 ) = 1.0;
    I( 1, 1 ) = 1.0;
    I( 2, 2 ) = 1.0;
    I( 3, 3 ) = 1.0;
    I( 4, 4 ) = 1.0;
    I( 5, 5 ) = 1.0;

    SMatrix66  Priori;
    SMatrix66* PrioriRecord = new SMatrix66[num_of_oriHits];
    SMatrix66  Posteriori;
    SMatrix66* PosterioriRecord = new SMatrix66[num_of_oriHits];
    SMatrix63  KalmanGain;
    SMatrix66  SmoothGain;
    SMatrix66  NoiseThMatrix;
    SMatrix33  NoiseMsMatrix;
    SMatrix33  KIn;

    //参数初始化
    Posteriori( 0, 0 ) = 1.0;
    Posteriori( 1, 1 ) = 1.0;
    Posteriori( 2, 2 ) = 1.0;
    Posteriori( 3, 3 ) = 1.0;
    Posteriori( 4, 4 ) = 1.0;
    Posteriori( 5, 5 ) = 1.0;

    PosterioriRecord[0] = Posteriori;

    KFx[0]  = ori_fx[0];
    KFy[0]  = ori_fy[0];
    KFz[0]  = ori_fz[0];
    KFvx[0] = speedx_group[0]; // speedx_group[0]
    KFvy[0] = speedy_group[0]; // speedy_group[0]
    KFvz[0] = speedz_group[0]; // speedz_group[0]

    //卡尔曼滤波过程for循环
    for ( int n2 = 1; n2 < num_of_oriHits; n2++ ) {
      Speedx = speedx_group[n2 - 1];
      Speedy = speedy_group[n2 - 1];
      Speedz = speedz_group[n2 - 1];

      Kalmanresult[0] = KFx[n2 - 1];
      Kalmanresult[1] = KFy[n2 - 1];
      Kalmanresult[2] = KFz[n2 - 1];
      Kalmanresult[3] = KFvx[n2 - 1];
      Kalmanresult[4] = KFvy[n2 - 1];
      Kalmanresult[5] = KFvz[n2 - 1];

      F( 0, 0 ) = 1.0;
      F( 0, 3 ) = dLz[n2 - 1] / Speedz; // dt
      F( 1, 1 ) = 1.0;
      F( 1, 4 ) = dLz[n2 - 1] / Speedz;
      F( 2, 2 ) = 1.0;
      F( 2, 5 ) = dLz[n2 - 1] / Speedz;
      F( 3, 3 ) = 1.0;
      F( 4, 4 ) = 1.0;
      F( 5, 5 ) = 1.0;

      Expectation = F * Kalmanresult;

      ExpectationRecord[n2] = Expectation; // x(k+1)=x(k)+vx*dt

      // float thetarnd0 = 13600. / e_energy * sqrt(DLz[n2-1]/23520.);
      NoiseThMatrix( 0, 0 ) = 0.0;
      NoiseThMatrix( 1, 1 ) = 0.0;
      NoiseThMatrix( 2, 2 ) = 0.0;
      NoiseThMatrix( 3, 3 ) = 2.0 * sin( thetarnd0 / 2 ) * sqrt( 0.5 * ( pow( Speedy, 2 ) + pow( Speedz, 2 ) ) );
      NoiseThMatrix( 4, 4 ) = 2.0 * sin( thetarnd0 / 2 ) * sqrt( 0.5 * ( pow( Speedx, 2 ) + pow( Speedz, 2 ) ) );
      NoiseThMatrix( 5, 5 ) = 2.0 * sin( thetarnd0 / 2 ) * sqrt( 0.5 * ( pow( Speedx, 2 ) + pow( Speedy, 2 ) ) );

      Priori = F * Posteriori * ROOT::Math::Transpose( F ) + NoiseThMatrix * NoiseThMatrix;

      PrioriRecord[n2] = Priori; // every state save in PrioriRecord

      NoiseMsMatrix( 0, 0 ) = m_errorX;
      NoiseMsMatrix( 1, 1 ) = m_errorY;
      NoiseMsMatrix( 2, 2 ) = m_errorZ;

      // cout<<"NoiseMsMatrix2: "<<NoiseMsMatrix * NoiseMsMatrix<<endl;

      KIn = measureMatrix * Priori * ROOT::Math::Transpose( measureMatrix ) + NoiseMsMatrix * NoiseMsMatrix;

      KIn.Invert();

      KalmanGain = Priori * ROOT::Math::Transpose( measureMatrix ) * KIn;

      XMscoli[0] = ori_fx[n2];
      XMscoli[1] = ori_fy[n2];
      XMscoli[2] = ori_fz[n2];

      Kalmanresult = Expectation + KalmanGain * ( XMscoli - measureMatrix * Expectation ); // Kalman result

      Posteriori = ( I - KalmanGain * measureMatrix ) * Priori;

      PosterioriRecord[n2] = Posteriori;

      KFx[n2]  = Kalmanresult[0];
      KFy[n2]  = Kalmanresult[1];
      KFz[n2]  = Kalmanresult[2];
      KFvx[n2] = Kalmanresult[3];
      KFvy[n2] = Kalmanresult[4];
      KFvz[n2] = Kalmanresult[5];

      NUM[n2] = n2;
    }
    //卡尔曼滤波过程结束

    //平滑过程开始
    SmoothResult[0]         = KFx[num_of_oriHits - 1];
    SmoothResult[1]         = KFy[num_of_oriHits - 1];
    SmoothResult[2]         = KFz[num_of_oriHits - 1];
    SmoothResult[3]         = KFvx[num_of_oriHits - 1];
    SmoothResult[4]         = KFvy[num_of_oriHits - 1];
    SmoothResult[5]         = KFvz[num_of_oriHits - 1];
    SRx[num_of_oriHits - 1] = KFx[num_of_oriHits - 1];
    SRy[num_of_oriHits - 1] = KFy[num_of_oriHits - 1];
    SRz[num_of_oriHits - 1] = KFz[num_of_oriHits - 1];
    for ( int n3 = num_of_oriHits - 2; n3 > -1; n3-- ) {
      Kalmanresult[0] = KFx[n3];
      Kalmanresult[1] = KFy[n3];
      Kalmanresult[2] = KFz[n3];
      Kalmanresult[3] = KFvx[n3];
      Kalmanresult[4] = KFvy[n3];
      Kalmanresult[5] = KFvz[n3];

      PrioriRecord[n3 + 1].Invert();

      F( 0, 0 ) = 1.0;
      F( 0, 3 ) = dLz[n3] / speedz_group[n3];
      F( 1, 1 ) = 1.0;
      F( 1, 4 ) = dLz[n3] / speedz_group[n3];
      F( 2, 2 ) = 1.0;
      F( 2, 5 ) = dLz[n3] / speedz_group[n3];
      F( 3, 3 ) = 1.0;
      F( 4, 4 ) = 1.0;
      F( 5, 5 ) = 1.0;

      SmoothGain = PosterioriRecord[n3] * ROOT::Math::Transpose( F ) * PrioriRecord[n3 + 1];

      SmoothResult = Kalmanresult + SmoothGain * ( SmoothResult - ExpectationRecord[n3 + 1] );

      SRx[n3]  = SmoothResult[0];
      SRy[n3]  = SmoothResult[1];
      SRz[n3]  = SmoothResult[2];
      SRvx[n3] = SmoothResult[3];
      SRvy[n3] = SmoothResult[4];
      SRvz[n3] = SmoothResult[5];
    }
    //平滑过程结束，读出数据

    for ( int k = 0; k < sizeof( SRx ) / sizeof( SRx[0] ); k++ ) {
      float nol_SRv[3];
      nol_SRv[0] = SRvx[k];
      nol_SRv[1] = SRvy[k];
      nol_SRv[2] = SRvz[k];

      float mag_a = 0;
      for ( int l = 0; l < 3; l++ ) { mag_a += pow( nol_SRv[l], 2 ); }
      for ( int h = 0; h < 3; h++ ) { nol_SRv[h] = nol_SRv[h] / mag_a; }

      KF_x.push_back( SRx[k] );
      KF_y.push_back( SRy[k] );
      KF_z.push_back( SRz[k] );
      // cout << " SRdata: " << k << ", position: (" << SRx[k] << ", " << SRy[k] << ", " << SRz[k]
      // << "), vector: (" << nol_SRv[0] << ", " << nol_SRv[1] << ", " << nol_SRv[2] << ")" <<endl;
    }

    delete[] ExpectationRecord;
    delete[] PrioriRecord;
    delete[] PosterioriRecord;

    ori_fx.clear();
    ori_fy.clear();
    ori_fz.clear();
    dLx.clear();
    dLy.clear();
    dLz.clear();
    speedx_group.clear();
    speedy_group.clear();
    speedz_group.clear();

    /// fill in the output collection
    int i = 0;
    for ( const auto& hit : *in_hits ) {
      auto new_hit = hit.clone();
      new_hit.setPosition( { KF_x[i], KF_y[i], KF_z[i] } );

      // add new hit to output collection
      out_hits->push_back( new_hit );
      i++;
    }

    return StatusCode::SUCCESS;
  }

private:
  /// Handle to EDM collection
  // DataHandle<edm4hep::SimTrackerHitCollection> m_inHits{ "TpcSimHits", Gaudi::DataHandle::Reader, this };
  // DataHandle<edm4hep::SimTrackerHitCollection> m_outHits{ "TpcKfHits", Gaudi::DataHandle::Writer, this };
  DataHandle<edm4hep::TrackerHitCollection> m_inHits{ "TpcCIHits", Gaudi::DataHandle::Reader, this };
  //  DataHandle<edm4hep::TrackerHitCollection> m_digiHits{ "TpcHits", Gaudi::DataHandle::Reader, this };
  DataHandle<edm4hep::TrackerHitCollection> m_outHits{ "TpcKfHits", Gaudi::DataHandle::Writer, this };

  /// Properties
  Gaudi::Property<double> m_errorX{ this, "error_x", 0.3, "[mm] measurement error in X-axis" };
  Gaudi::Property<double> m_errorY{ this, "error_y", 0.3, "[mm] measurement error in Y-axis" };
  Gaudi::Property<double> m_errorZ{ this, "error_z", 0.3, "[mm] measurement error in Z-axis" };

  /// random number
  TRandom3 m_gaus;
};

DECLARE_COMPONENT( KalmanFilterDemoAlg )
