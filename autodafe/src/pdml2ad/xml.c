/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : xml.c
 * DESCRIPTION: All the functions using the libxml2 library
 * DEPENDANCES: libxml2 - http://www.xmlsoft.org
 * COMPILATION: gcc -L/usr/include/libxml2 -lxml2 -o file file.c
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libxml/xmlmemory.h> // libxml2 */
#include <libxml/parser.h>    // libxml2 */

#include "debug.h"
#include "conf.h"
#include "output.h"
#include "recover.h"
#include "../../include/autodafe.h"

/*---------------------------------------------------------------------------*
 * NAME: xml_parse_field
 * DESC: Parse the content of a PDML proto field using the level (in the tree)
 *       the name of the selected field, and the name of the attribute (attr)
 * WARN: You have to free the returned pointer! ******************************
 *---------------------------------------------------------------------------*/
xmlChar * xml_parse_field(xmlNodePtr cur_orig, unsigned int level, char *name, char *attr) {
  
  int i;
  xmlChar *value;
  xmlChar *attrib;

  xmlNodePtr cur;
  cur = cur_orig;

  debug(3,"we are looking for %s->%s\n", name, attr);

  /* down to level */
  for(i=0;i<level;i++)
    cur = cur->xmlChildrenNode;

  while (cur != NULL) {
    if (!(xmlStrcmp(cur->name, (const xmlChar *) "field"))) {

      /* check the name */
      attrib = xmlGetProp(cur, "name");
      if (attrib != NULL) {
	if (!strncmp(attrib, name, strlen(attrib))) {
	  
	  /* retrieve the attribute's value */
	  value = xmlGetProp(cur, attr);
	  debug(3, "%s->%s found: %s\n", name, attr, value);
	  xmlFree(attrib);
	  return value;
	} 
	xmlFree(attrib);
      }
    }
    cur = cur->next;
  }
  debug(3, "value not found!\n");
  return 0;
}


/*---------------------------------------------------------------------------*
 * NAME: xml_parse_raw_data
 * DESC: parse the raw data of a udp or tcp packet (try to recover strings)
 *---------------------------------------------------------------------------*/
void xml_parse_raw_data(config *conf, xmlNodePtr cur_orig) {

  xmlChar *value;
  int ret_value;

  /* check the data value */
  value = xml_parse_field(cur_orig, 0, "data", "value");
  if (!value) {
    debug(3, "no data information in this packet\n");
    return;
  }
   
  debug(3, "raw data: %s\n", value);
  
  if (conf->check_string) {
    ret_value = recover_raw_string(conf, value);

    /* it's sure not a string */
    if(ret_value == 0) {
      if (conf->ethereal_hex_style) {
	output_tab_(conf->tab, AD_F_HEX"(\n");
	output_tab_ether(conf->tab, value);
	output_tab_(conf->tab, ");\n");
      }
      else 
	output_tab_(conf->tab, AD_F_HEX"(%s);\n", value);

    }
    
    /* it can be a string without \x0a at the end */
    else if (ret_value == 1) {
      if (conf->ethereal_hex_style) {
	output_tab_(conf->tab, AD_F_HEX"(\n");
	output_tab_ether(conf->tab, value);
	output_tab_(conf->tab, ");\n");
      }
      else 
	output_tab_(conf->tab, AD_F_HEX"(%s);\n", value);
      
      

    }
    
    /* it's definitely a string */
    else if (ret_value == 100) { 
      /* to nothing, it's has been done by the function recover_raw_string */
    }
    
  }

  
  else {
    if (conf->ethereal_hex_style) {
      output_tab_(conf->tab, AD_F_HEX"(\n");
      output_tab_ether(conf->tab, value);
      output_tab_(conf->tab, ");\n");
    }
    else
      output_tab_(conf->tab, AD_F_HEX"(%s);\n", value);
  }
  
  /* free the value */
  xmlFree(value);
  
}


/*---------------------------------------------------------------------------*
 * NAME: xml_parse_proto
 * DESC: Parse the content of a PDML protocol (not tcp,udp,ip,eth,frame,etc.)
 *---------------------------------------------------------------------------*/
void xml_parse_proto(config *conf, xmlNodePtr cur_orig) {

  xmlNodePtr cur;
  xmlChar *name;
  xmlChar *value;
  xmlChar *show;
  xmlChar *size;
  xmlChar *pos;
  xmlChar *showname;

  int checked_string;
  unsigned int length;

  for(cur = cur_orig;cur;cur = cur->next) {
    if (cur->type == XML_ELEMENT_NODE) {

      /* take the attributes */
      name     = xmlGetProp(cur, "name");
      value    = xmlGetProp(cur, "value");
      show     = xmlGetProp(cur, "show");
      size     = xmlGetProp(cur, "size");
      pos      = xmlGetProp(cur, "pos");
      showname = xmlGetProp(cur, "showname");

      debug(2, "name    :%s\n", name);
      debug(2, "show    :%s\n", show);
      debug(2, "value   :%s\n", value);
      debug(2, "size    :%s\n", size);
      debug(2, "pos     :%s\n", pos);
      debug(2, "showname:%s\n", showname);
      
      /* has some children ? */
      if (cur->children) {
	conf->tab++;
	debug(3, "children of %s detected\n", name);
	
	/* create a new block */
	output_tab_(conf->tab, AD_F_BLOCK_BEGIN"(\"packet_%d.%d.%s.%s\");\n", 
		    conf->packet_counter,
		    conf->proto_counter,
		    pos, name);
	xml_parse_proto(conf,cur->children);
	output_tab_(conf->tab, AD_F_BLOCK_END"(\"packet_%d.%d.%s.%s\");\n", 
		    conf->packet_counter,
		    conf->proto_counter,
		    pos, name);
	conf->tab--;
      }

      /* leaf of the tree, we record the content of value */
      else {
	
	/* write in the output field */
	conf->tab++;

	/* if the name, showname or show contain a " * / " it's a problem.. */
	output_tab_(conf->tab, "// name    : %s\n", name);
	output_tab_(conf->tab, "// showname: %s\n", showname);
	output_tab_(conf->tab, "// show    : %s\n", show);
	if (size)
	  output_tab_(conf->tab, "// size: 0x%s (%d)\n", size, strtoul(size, NULL, 16));

	/* yes, it happens! */
	else {
	  output_tab_(conf->tab, "// size: 0x0 (0)\n");
	  goto xml_parse_proto_end;
	}
	
	/* yes, it happens! */
	if (!value)
	  goto xml_parse_proto_end;

	/* check if we can convert the hexadecimal value in lenght. */
	/* you can disable this function with the check_length opt  */	
	/* WARNING! probabilistic analysis!                         */
	length = 0;
	if ((conf->check_length)) {
	  debug(1, "check if the value can be a length field\n");
	  debug(1, "WARNING! not 100%% sure! you need to verify!\n");
	  length = recover_length(conf, name, show, value, showname);
	  if (length)
	    goto xml_parse_proto_end; 
	}
	debug(1, "the value is not a length\n");



	/* check if we can convert the hexadecimal value in a string */
	/* you can disable this function with the check_string opt   */
	/* WARNING! probabilistic analysis! */
	checked_string = 0;
	if ((conf->check_string)) {
	  debug(1, "check if the value can be converted in a string\n");
	  checked_string = recover_string(conf, name, show, value, showname);
	}
	if (checked_string)
	  goto xml_parse_proto_end;
	debug(1, "the value is not a string\n");

	/* the value is considered as an hexadecimal value. However, */
	/* you can write this value using the Ethereal-like style    */
	/* which is activated by default (more userfriendly)         */
	if (conf->ethereal_hex_style) {
	  output_tab_(conf->tab, AD_F_HEX"(\n");
	  output_tab_ether(conf->tab, value);
	  output_tab_(conf->tab, ");\n");
	}
	else
	  output_tab_(conf->tab, AD_F_HEX"(%s);\n", value);
	
      xml_parse_proto_end:
	conf->tab--;
      }
      
      /* free attributes */
      if (name) xmlFree(name);
      if (value) xmlFree(value);
      if (show) xmlFree(show);
      if (size) xmlFree(size);
      if (pos) xmlFree(pos);
      if (showname) xmlFree(showname);
    }
  }
}


/*---------------------------------------------------------------------------*
 * NAME: xml_parse_ip
 * DESC: Parse the content of a PDML ip 
 *---------------------------------------------------------------------------*/
void xml_parse_ip(config *conf, xmlNodePtr cur_orig) {
  
  xmlChar *value;
  unsigned long result1;
  unsigned long result2;

  /* look for the value of "ip.src" 1 down to the tree */
  value = xml_parse_field(cur_orig, 1, "ip.src", "value");
  if (!value) exit(-1);

  /* convert the value in unsigned long */
  result1 = strtoul((char *)value, NULL, 16);

  /* free the value */
  xmlFree(value);

  /* look for the value of "ip.dst" 1 down to the tree */
  value = xml_parse_field(cur_orig, 1, "ip.dst", "value");
  if (!value) exit(-1);

  /* convert the value in unsigned long */
  result2 = strtoul((char *)value, NULL, 16);
  debug(2, "value: %s\n", value);
  debug(2, "result2: %d\n", result2);

  /* free the value */
  xmlFree(value);

  if (conf->ip_client == 0) conf->ip_client = result1;
  if (conf->ip_server == 0) conf->ip_server = result2;
  debug(2, "ip of the client: %p\n", conf->ip_client);
  debug(2, "ip of the server: %p\n", conf->ip_server);
  debug(2, "result1: %p\n", result1);
  debug(2, "result2: %p\n", result2);

  /* now we write the ip of the packet */
  conf->ip_pkt = result1;  
  
  /* check to be sure */
  if (conf->ip_pkt != conf->ip_client) {
    if (conf->ip_pkt != conf->ip_server) {
      error_("more than two IP address! Check your Ethereal logs!\n");
      exit(-1);
    }
  }
  

  
}

/*---------------------------------------------------------------------------*
 * NAME: xml_parse_packet
 * DESC: Parse the content of a PDML packet
 *---------------------------------------------------------------------------*/
void xml_parse_packet(config *conf) {
  
  xmlChar *name;

  /* we keep a copy of cur */
  xmlNodePtr cur = conf->cur;

  /* enter in packet */
  cur = cur->xmlChildrenNode;

  /* init the protocol counter */
  conf->proto_counter = 1;
  debug(3, "protocol counter: %d\n", conf->proto_counter);

  while (cur != NULL) {
    if (!(xmlStrcmp(cur->name, (const xmlChar *) "proto"))) {

      /* catch the name of the proto */
      name = xmlGetProp(cur, "name");

      /* geninfo type */
      if (!strncmp(name, "geninfo", strlen(name))) {
	debug(3, "retreiving geninfo informations.\n", name);
      }

      /* frame type */
      else if (!strncmp(name, "frame", strlen(name))) {
	debug(3, "retreiving frame informations.\n", name);
      }

      /* eth type */
      else if (!strncmp(name, "eth", strlen(name))) {
	debug(3, "retreiving eth informations.\n", name);
      }

      /* ip type */
      else if (!strncmp(name, "ip", strlen(name))) {
	debug(3, "retreiving ip informations.\n", name);
	xml_parse_ip(conf, cur);

      }

      /* tcp type */
      else if (!strncmp(name, "tcp", strlen(name))) {
	debug(3, "retreiving tcp informations.\n", name);
	
	/* the communication seems to be in tcp */
	if (!conf->transport_type)  conf->transport_type = 1;

	/* udp and tcp in the same sniff ? huh not good! */
	else if (conf->transport_type != 1) {
	  error_("UDP *AND* TCP?! Check your Ethereal logs!\n");				  
	  exit(-1);
	}


	/* save the port (for identification, ip is not enough (think localhost)) */
	xmlChar *value;
	unsigned short src_port;
	unsigned short dst_port;
	
	value = xml_parse_field(cur, 1, "tcp.srcport", "show");
	src_port = (unsigned short) atoi(value);
	xmlFree(value);
	
	value = xml_parse_field(cur, 1, "tcp.dstport", "show");
	dst_port = (unsigned short) atoi(value);
	xmlFree(value);

	/* save the pkt source port */
	conf->port_pkt = src_port;
	
	/* save the port */
	if ((conf->port_client == 0) && (conf->port_server == 0)) {
	  if (conf->ip_client == conf->ip_pkt) {
	    conf->port_client = src_port;
	    conf->port_server = dst_port;
	  }
	  if (conf->ip_server == conf->ip_pkt) {
	    conf->port_client = dst_port;
	    conf->port_server = src_port;
	  }  
	}
      }

      /* udp type */
      else if (!strncmp(name, "udp", strlen(name))) {
	debug(3, "retreiving udp informations.\n", name);
	
	/* the communication seems to be in udp */
	if (!conf->transport_type) conf->transport_type = 2;

	/* udp and tcp in the same sniff ? huh not good! */
	else if (conf->transport_type != 2) {
	  error_("UDP *AND* TCP?! Check your Ethereal logs!\n");				  
	  exit(-1);
	}

	/* save the port (for identification, ip is not enough (think localhost)) */
	xmlChar *value;
	unsigned short src_port;
	unsigned short dst_port;
	
	value = xml_parse_field(cur, 1, "udp.srcport", "show");
	src_port = (unsigned short) atoi(value);
	xmlFree(value);
	
	value = xml_parse_field(cur, 1, "udp.dstport", "show");
	dst_port = (unsigned short) atoi(value);
	xmlFree(value);

	/* save the pkt source port */
	conf->port_pkt = src_port;
	
	/* save the port */
	if ((conf->port_client == 0) && (conf->port_server == 0)) {
	  if (conf->ip_client == conf->ip_pkt) {
	    conf->port_client = src_port;
	    conf->port_server = dst_port;
	  }
	  if (conf->ip_server == conf->ip_pkt) {
	    conf->port_client = dst_port;
	    conf->port_server = src_port;
	  }  
	}
      }

      /* other packet */
      /* TODO XXXFIXMEXXX parse icmp, etc.. like tcp */
      else if (conf->check_proto) {
	debug(3, "proto type: %s\n", name);
	
	/* parse the content of the packet */
	verbose_("[*] packet type: %s\n", name);
	xml_parse_proto(conf, cur);
      }

      conf->proto_counter++;

      /* free the field name */
      xmlFree(name);
    }
    else if (!(xmlStrcmp(cur->name, (const xmlChar *) "field"))) {
      if (!conf->check_proto) xml_parse_raw_data(conf, cur);
    }
    cur = cur->next;
  }
}

/*---------------------------------------------------------------------------*
 * NAME: xml_parsing
 * DESC: Main xml parsing function
 *---------------------------------------------------------------------------*/
void xml_parsing(config *conf) {

  /* open the file using the libxml2 library */
  debug(3, "xml parsing of the file: %s\n", conf->xml_filename);
  conf->doc = xmlParseFile(conf->xml_filename);
  if (conf->doc == NULL) {
    error_("Document not parsed successfully!\n");
    return;
  }
  verbose_("[*] parsing the file: %s\n", conf->xml_filename);

  /* point on the root element of the tree */
  conf->cur = xmlDocGetRootElement(conf->doc);
  if (conf->cur == NULL) {
    error_("Empty Document!\n");
    xmlFreeDoc(conf->doc);
    return;
  }
  
  /* check if the format seems to be PDML: ie root = pdml */
  debug(3,"name of the root: %s\n", conf->cur->name);
  if (xmlStrcmp(conf->cur->name, (const xmlChar *) "pdml")) {
    error_("Document NOT a PDML file!\n");
    xmlFreeDoc(conf->doc);
    return;
  }

  /* start the autodafe's script language */
  output_("/*--------------------------------------------------------------------------*\n");
  output_(" * xml autodafe's parser v.%d.%d (c) Martin Vuagnoux - 2004-2006            *\n", MAJOR_VERSION, MINOR_VERSION);
  output_(" * auto-generated script using PDML (Packet Details Markup Language) source *\n");
  output_(" *--------------------------------------------------------------------------*/\n\n");

  /* the file is good, check for the packet entry (child) */
  conf->cur = conf->cur->xmlChildrenNode;
  while (conf->cur != NULL) {
    if (!xmlStrcmp(conf->cur->name, (const xmlChar *) "packet")) {
      debug(3, "packet field found\n");
      /* open the block */
      output_(AD_F_BLOCK_BEGIN"(\"packet_%d\");\n", conf->packet_counter);
      
      /* parse the internal data of the packet block */
      xml_parse_packet(conf);
      
      /* close the block */
      output_(AD_F_BLOCK_END"(\"packet_%d\");\n", conf->packet_counter);
      
      /* client send/recv */
      if ((conf->ip_pkt == conf->ip_client) && (conf->port_pkt == conf->port_client)) {
	debug(3, "check client\n");
	if (conf->transport_type == 1) {
	  if (conf->invert == 0)
	    output_(AD_F_SEND"(\"packet_%d\");  /* tcp */\n\n", conf->packet_counter);
	  else
	    output_(AD_F_RECV"(\"packet_%d\");  /* tcp */\n\n", conf->packet_counter);
	}
	else if (conf->transport_type == 2) {
	  if (conf->invert == 0)
	    output_(AD_F_SEND"(\"packet_%d\");  /* udp */\n\n", conf->packet_counter);
	  else
	    output_(AD_F_RECV"(\"packet_%d\");  /* udp */\n\n", conf->packet_counter);
	}
      }
      
      /* server send/recv */
      else if ((conf->ip_pkt == conf->ip_server) && (conf->port_pkt == conf->port_server)) {
	debug(3, "check server\n");
	if (conf->transport_type == 1) {
	  if (conf->invert == 1)
	    output_(AD_F_SEND"(\"packet_%d\");  /* tcp */\n\n", conf->packet_counter);
	  else
	    output_(AD_F_RECV"(\"packet_%d\");  /* tcp */\n\n", conf->packet_counter);
	}
	else if (conf->transport_type == 2) {
	  if (conf->invert == 1)
	    output_(AD_F_SEND"(\"packet_%d\");  /* udp */\n\n", conf->packet_counter);
	  else
	    output_(AD_F_RECV"(\"packet_%d\");  /* udp */\n\n", conf->packet_counter);
	}
      }
      
      else {
	error_("too much different packets?! Check your Ethereal logs!\n");
	error_("client: ip->%d, port->%d\n", conf->ip_client, conf->port_client);
	error_("server: ip->%d, port->%d\n", conf->ip_server, conf->port_server);
	error_("packet: ip->%d, port->%d\n", conf->ip_pkt, conf->port_pkt);
	exit(-1);
      }
      
      conf->packet_counter++;      
    }
    conf->cur = conf->cur->next;
  }
  
  /* free */
  xmlFreeDoc(conf->doc);
}
