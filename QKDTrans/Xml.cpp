/*
	$Id: Xml.cpp 624 2012-06-29 04:22:44Z lwy $
	$URL: http://172.17.8.1/svn/QUESS/tags/QKDBench-2.2.1/Xml.cpp $
*/
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include "xml.h"

CXML::CXML()
{
	xmlCharEncodingHandlerPtr handler;
	m_xmlDocPtr = NULL;
	handler = xmlFindCharEncodingHandler("GB2312");
	if(!handler){
		xmlNewCharEncodingHandler("GB2312",CXML::gbk_input,CXML::gbk_output);
	}
	m_xpathCtx = NULL;
}

CXML::~CXML()
{
	if(!m_xpathCtx){
		xmlXPathFreeContext(m_xpathCtx);
	}
	if(m_xmlDocPtr){
		xmlFreeDoc(m_xmlDocPtr);
	}
	m_xpathCtx = NULL;
	m_xmlDocPtr = NULL;
}

void CXML::Open(char *file)
{
	m_xmlDocPtr = xmlParseFile(file);
	if(m_xmlDocPtr){
		m_xpathCtx = xmlXPathNewContext(m_xmlDocPtr);
	}
}

xmlXPathObjectPtr CXML::GetNodesByXPath(const xmlChar *expr)
{
	if(!m_xpathCtx){
		return NULL;
	}
	return xmlXPathEvalExpression(expr,m_xpathCtx);
}

int CXML::code_convert(char *from_charset, char *to_charset, char *in, int *inlen, char *out, int *outlen)
{
	iconv_t cd;
	char *outbuf = (char *)out;
	char *inbuf = (char *)in;
	
	cd = iconv_open(to_charset, from_charset);
	if(!cd){
		return -1;
	}
	
	memset(outbuf,0,*outlen);
	
	if(iconv(cd, (const char **)&inbuf, (unsigned int *)inlen, &outbuf, (unsigned int *)outlen) == -1){
		return -1;
	}
	
	*outlen = outbuf - out;
	*inlen = inbuf - in;
	
	iconv_close(cd);
	
	return 0;
}

int CXML::gbk_input(unsigned char *out, int *outlen, const unsigned char *in, int *inlen)
{
	
	if(code_convert("GB2312","UTF-8",(char *)in,inlen,(char *)out,outlen) == -1){
		return -1;
	}
	
	return(*outlen);
}

int CXML::gbk_output(unsigned char *out, int *outlen, const unsigned char *in, int *inlen)
{
	if(code_convert("UTF-8","GB2312",(char *)in,inlen,(char *)out,outlen) == -1){
		return -1;
	}
	
	return(*outlen);
}

xmlChar *CXML::g2u(xmlChar *input)
{
	if(!input){
		return NULL;
	}

	xmlChar *output;
	int in_size,out_size;
	in_size = strlen((char *)input);
	out_size = strlen((char *)input) * 2 + 1;
	output = (xmlChar *)xmlMalloc((size_t)out_size);

	if(gbk_input(output,&out_size,input,&in_size) == -1){
		return NULL;
	}
	output = (xmlChar *)xmlRealloc(output,out_size+1);
	output[out_size] = 0;
	return output;
}

xmlChar * CXML::xmlGetPropGBK(xmlNodePtr node, const xmlChar *name)
{
	xmlChar *pUTFProp,*pGBKProp;
	int in_size, out_size;
	
	pUTFProp = xmlGetProp(node,name);
	
	if(!pUTFProp){
		return NULL;
	}
	
	in_size = strlen((char *)pUTFProp);
	out_size = strlen((char *)pUTFProp) * 2 + 1;
	
	pGBKProp = (xmlChar *)xmlMalloc((size_t)out_size);
	
	if(gbk_output(pGBKProp,&out_size,pUTFProp,&in_size) == -1){
		return NULL;
	}
	pGBKProp = (xmlChar *)xmlRealloc(pGBKProp,out_size+1);
	pGBKProp[out_size] = 0;

	xmlFree(pUTFProp);
	return pGBKProp;
}
