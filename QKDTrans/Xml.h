/*
	$Id: Xml.h 624 2012-06-29 04:22:44Z lwy $
	$URL: http://172.17.8.1/svn/QUESS/tags/QKDBench-2.2.1/Xml.h $
*/
#ifndef __XML__H__
#define __XML__H__

#include "libxml/parser.h"
#include "libxml/xmlmemory.h"
#include "libxml/xpath.h"

class CXML
{
private:
	xmlDocPtr m_xmlDocPtr;
	xmlXPathContextPtr m_xpathCtx;

public:
	CXML();
	~CXML();
	static int code_convert(char *from_charset, char *to_charset, char *in, int *inlen, char *out, int *outlen);
	static int gbk_input(unsigned char *out, int *outlen, const unsigned char *in, int *inlen);
	static int gbk_output(unsigned char *out, int *outlen, const unsigned char *in, int *inlen);
	static xmlChar *g2u(xmlChar *input);
	static xmlChar * xmlGetPropGBK(xmlNodePtr node, const xmlChar *name);

	void Open(char *file);
	xmlXPathObjectPtr GetNodesByXPath(const xmlChar *expr);


};

#endif