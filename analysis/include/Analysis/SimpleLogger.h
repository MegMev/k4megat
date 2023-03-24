#ifndef MEGAT_SimpleLogger_H
#define MEGAT_SimpleLogger_H

#include <cstdio>

#define LOG_INFO( format, ... ) fprintf( stdout, "%s:%d|" format "\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#define LOG_DBG( format, ... ) fprintf( stdout, "%s:%d|" format "\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#define LOG_ERR( format, ... ) fprintf( stderr, "%s:%d|" format "\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#define LOG_WARN( format, ... ) fprintf( stdout, "%s:%d|" format "\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#define LOG_TRACE( format, ... ) fprintf( stdout, "%s:%d|" format "\n", __FILE__, __LINE__, ##__VA_ARGS__ )

#endif /* MEGAT_SimpleLogger_H */
