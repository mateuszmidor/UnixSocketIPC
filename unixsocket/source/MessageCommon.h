/**
 * MessageCommon.h
 *
 *  Created on: Jun 22, 2016
 *      Author: Mateusz Midor
 */

#ifndef UNIXSOCKET_SOURCE_MESSAGECOMMON_H_
#define UNIXSOCKET_SOURCE_MESSAGECOMMON_H_

namespace unixsocketipc {

//#define DEBUG_ON

#ifdef DEBUG_ON
#define DEBUG_MSG(...) printf(__VA_ARGS__)
#else
#define DEBUG_MSG(...)
#endif

const unsigned STOP_LISTENING_MSG_ID = 0;
const unsigned MESSAGE_BUFF_SIZE = 1024 * 1024 * 1; // 1MB

}

#endif /* UNIXSOCKET_SOURCE_MESSAGECOMMON_H_ */
