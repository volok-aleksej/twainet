#ifndef NET_STRUCTS_H
#define	NET_STRUCTS_H

//ether types
#define ETHERTYPE_IP		0x0800	//IPv4 protocol
#define ETHERTYPE_IPv6		0x86dd	//IPv6 protocol
#define ETHERTYPE_PPPOED	0x8863	//PPP over Ethernet Discovery protocol
#define ETHERTYPE_PPPOES	0x8864	//PPP over Ethernet Session protocol
#define ETHERTYPE_PPPIPCP	0x8021	//PPP IP Control protocol
#define ETHERTYPE_PPPLCP	0xc021	//PPP Link Control protocol
#define ETHERTYPE_PPPPA		0xc023	//PPP Password Autentification protocol
#define ETHERTYPE_PPPCC		0x80fd	//PPP Compression Control protocol

/************************************************************************************************/
/*																								*/
/*                 _ether_header			data												*/
/*				|----------------------|----------------------|									*/
/*              |                      |                      |									*/
/*				|----------------------|----------------------|									*/
/*																								*/
/************************************************************************************************/

typedef struct _ether_header
{
	unsigned char	ether_dhost[6];
	unsigned char	ether_shost[6];
	unsigned short	ether_type;		//see ether types
} ether_header;


/************************************************************************************************/
/*																								*/
/*                 _pppoe_header			_pppoe_tag_header		data	   ...				*/
/*				|----------------------|----------------------|------------|					*/
/*              |                      |                      |            |   ...				*/
/*				|----------------------|----------------------|------------|					*/
/*																								*/
/************************************************************************************************/
//PPPOED tags
#define PPPOED_EOF		0x000	//End of List
#define PPPOED_SN		0x101	//Service Name
#define PPPOED_AN		0x102	//AC Name
#define PPPOED_HU		0x103	//Host Uniq
#define PPPOED_ACC		0x104	//AC Coockie
#define PPPOED_VS		0x105	//Vendor Specific
#define PPPOED_CR		0x106	//Credits
#define PPPOED_MT		0x107	//Metrics
#define PPPOED_SQN		0x108	//Sequence Number
#define PPPOED_CSF		0x109	//Credit Scale Number
#define PPPOED_RSI		0x110	//Relay Session Id
#define PPPOED_HURL		0x111	//HURL
#define PPPOED_MOTM		0x112	//MOTM
#define PPPOED_PPPMP	0x120	//PPP Max Payload
#define PPPOED_IPRA		0x121	//IP Route Add
#define PPPOED_SNE		0x201	//Service Name Error
#define PPPOED_ACSE		0x202	//AC System Error
#define PPPOED_GE		0x203	//Generic Error

#define PPPOED_VENDOR	"TwainetPPP"

#define PPPOE_VERTYPE	0x11

typedef struct _pppoe_tag_header
{
	unsigned short name;	//see PPPOED tags
	unsigned short length;	//length of data
} pppoe_tag;

typedef struct _pppoe_header
{
	unsigned char version_type;	// version(HIWORD), type(LOWORD), version_type = PPPOE_VERTYPE
	unsigned char code;			//PPPOED code
	unsigned short sessionId;
	unsigned short payload;		//length of pppoe tag headers
} pppoe_header;

#endif/*NET_STRUCTS_H*/
