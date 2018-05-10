//
// mqttconfig.h
//
#ifndef _mqttconfig_h
#define _mqttconfig_h

#define MQTT_BROKER_HOSTNAME	"test.mosquitto.org"

#define PUB_CLIENT_ID		"circle:pub:001"
#define PUB_USER_NAME		0
#define PUB_PASSWORD		0
#define PUB_TOPIC		"circle/status/json"

#define SUB_CLIENT_ID		"circle:sub:001"
#define SUB_USER_NAME		0
#define SUB_PASSWORD		0
#define SUB_TOPIC		"circle/status/json"

// See: include/circle/net/mqttclient.h
#define MAX_PACKET_SIZE		1024
#define MAX_PACKETS_QUEUED	4
#define MAX_TOPIC_SIZE		256

#endif
