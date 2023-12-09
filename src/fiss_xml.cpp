
#include "fiss_xml.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if	defined(_MSC_VER)
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#ifndef FISS_XML_PATH
#define FISS_XML_PATH "data\\SKSE\\Plugins\\FISS"
#endif

#ifndef FISS_XML_VER
#define FISS_XML_VER "1.2"
#endif

#ifndef FISS_HEADER
#define FISS_HEADER "<fiss><Header><Version>%s</Version><ModName>"
#endif

#ifndef FISS_HEADER_END
#define FISS_HEADER_END "</ModName></Header><Data>"
#endif

#ifndef FISS_END_FILE
#define FISS_END_FILE "</Data></fiss>"
#endif

static const char FISS_EMPTY_STR[12] = {""};
static const char *FISS_ERR_MSG[] =
{
	"",
	"XML file is not existed",
	"Allocate memory failed",
	"Read XML file data failed",
	"Illegal FISS XML file format",
	"NULL object",
	"endSave() is not called last time",
	"endLoad() is not called last time",
	"Create XML/TXT file failed",
	"Cannot get current position in stream"
};

static bool is_uppercase(char cc)
{
	if (cc>='A' && cc<='Z') return true;
	return false;
}

static bool is_extension(const char* fname, const char* ename)
{
	int n;
	int elen = strlen(ename); 
	int flen = strlen(fname) - elen - 1;
	if (flen<=0) return true;
	if (fname[flen]!='.') return false;
	for (n=0; n<elen; n++)
	{
		char aa = fname[flen + 1 + n];
		char bb = ename[n];
		if (is_uppercase(aa)) aa += 0x20;
		if (is_uppercase(bb)) bb += 0x20;
		if (aa!=bb) return false;
	}
	return true;
}

static bool is_root_path(const char* path)
{
	#if defined(_MSC_VER)
	if (path[1] != ':')  return false;
	if (path[2] != '\\') return false;
	#else
	if (path[0] != '/')  return false;
	#endif
	return true;
}

static void fopen_mkdir(const char* name, bool is_path_ony)
{
	int i, offset=0, len=strlen(name);
	#if defined(_MSC_VER)
	char* mname = _strdup(name);
	#else
	char* mname = strdup(name);
	#endif

	if (mname==NULL) return;
	for (i=len-1; i>=0 && is_path_ony==false; i--)
	{
		if (mname[i]=='\\'||mname[i]=='/')
		{
			mname[i+1] = 0;
			break;
		}
	}
	if (is_root_path(name))
	{
		#if defined(_MSC_VER)
		offset = 3;
		#else
		offset = 1;
		#endif
	}
	for (i=offset; mname[i]!=0; i++)
	{
		int ret = 0;
		char slash = mname[i];
		if (slash=='\\'||slash=='/')
		{
			mname[i] = 0;
			#if defined(_MSC_VER)
			ret = _mkdir(mname);
			#else
			ret = mkdir(mname, 0777);
			#endif
			mname[i] = slash;
		}
	}
	free(mname);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Load

void cfiss_beginLoad(CFISS* obj, const char* filename)
{
	char fname[1500], header[80], *buffer, *pch;
	long lSize, result;
	FILE *pFile;

	if (obj==NULL) return;
	if (obj->xmlData!=NULL)
	{
		obj->errMsg = FISS_ERR_MSG[7];
		return;
	}
	// open XML file
	if (is_root_path(filename))
	{
		snprintf(fname, sizeof(fname), "%s%s", filename, (is_extension(filename, "xml"))?"":".xml");
	}
	else
	{
		snprintf(fname, sizeof(fname), "%s\\%s%s", FISS_XML_PATH, filename, (is_extension(filename, "xml"))?"":".xml");
	}
	pFile = fopen(fname, "rb");
	if (pFile==NULL)
	{
		obj->errMsg = FISS_ERR_MSG[1];
		return;
	}
	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);
	if (lSize<=0)
	{
		fclose(pFile);
		obj->errMsg = FISS_ERR_MSG[9];
		return;
	}
	// allocate memory to contain the whole file:
	buffer = (char*)calloc(lSize+1, sizeof(char));
	if (buffer==NULL)
	{
		fclose(pFile);
		obj->errMsg = FISS_ERR_MSG[2];
		return;
	}
	// copy the file into the buffer
	result = fread(buffer, 1, lSize, pFile);
	if (result!=lSize)
	{
		free(buffer);
		fclose(pFile);
		obj->errMsg = FISS_ERR_MSG[3];
		return;
	}
	// terminate
	fclose(pFile);

	// check header
	snprintf(header, sizeof(header), FISS_HEADER, FISS_XML_VER);
	pch = strstr(buffer, header);
	if (pch==NULL)
	{
		free(buffer);
		obj->errMsg = FISS_ERR_MSG[4];
		return;
	}
	pch = strstr(buffer, FISS_HEADER_END);
	if (pch==NULL)
	{
		free(buffer);
		obj->errMsg = FISS_ERR_MSG[4];
		return;
	}
	pch += strlen(FISS_HEADER_END);

	// initialize
	obj->xmlData = buffer;
	obj->eOffset = 0;
	obj->Data = pch;
}
const char*	cfiss_loadString(CFISS* obj, const char* name)
{
	char vname[1500], *pch, *ret;

	if (obj==NULL) return FISS_EMPTY_STR;
	if (obj->xmlData==NULL) return FISS_EMPTY_STR;

	// recover "<"
	if (obj->eOffset>0)
	{
		obj->Data[obj->eOffset] = '<';
		obj->eOffset = 0;
	}
	// search "<name>"
	snprintf(vname, sizeof(vname), "<%s>", name);
	pch = strstr(obj->Data, vname); if (pch==NULL) return FISS_EMPTY_STR;
	ret = pch + strlen(vname);

	// search "</name>"
	snprintf(vname, sizeof(vname), "</%s>", name);
	pch = strstr(pch, vname); if (pch==NULL) return FISS_EMPTY_STR;
	obj->eOffset = pch - obj->Data;
	*pch = 0;
	return ret;
}
bool cfiss_loadBool(CFISS* obj, const char* name)
{
	const char* var = cfiss_loadString(obj, name);
	if (var[0]==0) return false;
	else if (var[0]=='0') return false;
	else if (var[0]=='1') return true;
	else if (!strcmp(var, "true")) return true;
	else if (!strcmp(var, "false")) return false;
	return false;
}
float cfiss_loadFloat(CFISS* obj, const char* name)
{
	const char* var = cfiss_loadString(obj, name);
	if (var[0]==0) return 0.0;
	return atof(var);
}
int cfiss_loadInt(CFISS* obj, const char* name)
{
	const char* var = cfiss_loadString(obj, name);
	if (var[0]==0) return 0;
	return atoi(var);
}
const char* cfiss_endLoad(CFISS* obj)
{
	if (obj==NULL)
	{
		return FISS_ERR_MSG[5];
	}
	if (obj->xmlData==NULL)
	{
		return (obj->errMsg)?obj->errMsg:FISS_EMPTY_STR;
	}
	free(obj->xmlData);
	memset(obj, 0, sizeof(CFISS));
	return FISS_EMPTY_STR;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save

void cfiss_beginSave(CFISS* obj, const char* filename, const char* modname)
{
	char msg[1500], header[80];
	FILE *pFile;

	if (obj==NULL) return;
	if (obj->io!=NULL)
	{
		obj->errMsg = FISS_ERR_MSG[6];
		return;
	}
	// create XML file
	if (is_root_path(filename))
	{
		snprintf(msg, sizeof(msg), "%s%s", filename, (is_extension(filename, "xml"))?"":".xml");
	}
	else 
	{
		snprintf(msg, sizeof(msg), "%s\\%s%s", FISS_XML_PATH, filename, (is_extension(filename, "xml"))?"":".xml");
	}
	fopen_mkdir(msg, false);
	pFile = fopen(msg, "wb");
	if (pFile==NULL)
	{
		obj->io = NULL;
		obj->errMsg = FISS_ERR_MSG[8];
		return;
	}
	// write FISS header
	snprintf(header, sizeof(header), FISS_HEADER, FISS_XML_VER);
	fwrite(header, sizeof(char), strlen(header), pFile);

	// write MOD name & end of FISS header
	fwrite(modname, sizeof(char), strlen(modname), pFile);
	fwrite(FISS_HEADER_END, sizeof(char), strlen(FISS_HEADER_END), pFile);

	// initialize
	obj->io = pFile;
	obj->errMsg = FISS_ERR_MSG[0];
}
void cfiss_saveBool(CFISS* obj, const char* name, bool value)
{
	char msg[1500];
	FILE *pFile;

	if (obj==NULL) return;
	if (name==NULL) return;
	if (obj->io==NULL) return;
	if (strlen(name)+3>=sizeof(msg)) return;

	pFile = (FILE*)obj->io;

	snprintf(msg, sizeof(msg), "<%s>", name);
	fwrite(msg, sizeof(char), strlen(msg), pFile);

	snprintf(msg, sizeof(msg), "%s", (value)?"1":"0");
	fwrite(msg, sizeof(char), strlen(msg), pFile);

	snprintf(msg, sizeof(msg), "</%s>", name);
	fwrite(msg, sizeof(char), strlen(msg), pFile);
}
void cfiss_saveString(CFISS* obj, const char* name, const char* value)
{
	char msg[1500];
	FILE *pFile;

	if (obj==NULL) return;
	if (name==NULL) return;
	if (value==NULL) return;
	if (obj->io==NULL) return;
	if (strlen(name)+3>=sizeof(msg)) return;

	pFile = (FILE*)obj->io;

	snprintf(msg, sizeof(msg), "<%s>", name);
	fwrite(msg, sizeof(char), strlen(msg), pFile);

	fwrite(value, sizeof(char), strlen(value), pFile);

	snprintf(msg, sizeof(msg), "</%s>", name);
	fwrite(msg, sizeof(char), strlen(msg), pFile);
}
void cfiss_saveFloat(CFISS* obj, const char* name, float value)
{
	char msg[1500];
	FILE *pFile;

	if (obj==NULL) return;
	if (name==NULL) return;
	if (obj->io==NULL) return;
	if (strlen(name)+3>=sizeof(msg)) return;

	pFile = (FILE*)obj->io;

	snprintf(msg, sizeof(msg), "<%s>", name);
	fwrite(msg, sizeof(char), strlen(msg), pFile);

	snprintf(msg, sizeof(msg), "%f", value);
	fwrite(msg, sizeof(char), strlen(msg), pFile);

	snprintf(msg, sizeof(msg), "</%s>", name);
	fwrite(msg, sizeof(char), strlen(msg), pFile);
}
void cfiss_saveInt(CFISS* obj, const char* name, int value)
{
	char msg[1500];
	FILE *pFile;

	if (obj==NULL) return;
	if (name==NULL) return;
	if (obj->io==NULL) return;
	if (strlen(name)+3>=sizeof(msg)) return;

	pFile = (FILE*)obj->io;

	snprintf(msg, sizeof(msg), "<%s>", name);
	fwrite(msg, sizeof(char), strlen(msg), pFile);

	snprintf(msg, sizeof(msg), "%d", value);
	fwrite(msg, sizeof(char), strlen(msg), pFile);

	snprintf(msg, sizeof(msg), "</%s>", name);
	fwrite(msg, sizeof(char), strlen(msg), pFile);
}
const char* cfiss_endSave(CFISS* obj)
{
	FILE *pFile;
	if (obj==NULL)
	{
		return FISS_ERR_MSG[5];
	}
	if (obj->io==NULL)
	{
		return (obj->errMsg)?obj->errMsg:FISS_EMPTY_STR;
	}
	pFile = (FILE*)obj->io;
	fwrite(FISS_END_FILE, sizeof(char), strlen(FISS_END_FILE), pFile);
	fclose(pFile);
	obj->io = NULL;
	return FISS_EMPTY_STR;
}
const char* cfiss_saveTextToTxtFile(const char* filename, const char* text)
{
	char fname[1500];
	FILE *pFile;

	if (is_root_path(filename))
	{
		snprintf(fname, sizeof(fname), "%s%s", filename, (is_extension(filename, "txt"))?"":".txt");
	}
	else
	{
		snprintf(fname, sizeof(fname), "%s\\%s%s", FISS_XML_PATH, filename, (is_extension(filename, "txt"))?"":".txt");
	}
	fopen_mkdir(fname, false);
	pFile = fopen(fname, "wb"); if (pFile==NULL) return FISS_ERR_MSG[8];
	fwrite(text, sizeof(char), strlen(text)+1, pFile);
	fclose(pFile);
	return FISS_EMPTY_STR;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Special

static bool is_valid_character(char ch)
{
	unsigned char uch = (unsigned char)ch;
	if      (0x01<=uch && uch<=0x1F) return false;
	else if (0x7F<=uch && uch<=0xFF) return false;
	else if (uch=='>' || uch=='<' || uch=='*' || uch=='\"'|| uch=='|' || uch=='?')
	return false;
	return true;
}
static char nv_to_hex(bool high, char ch)
{
	static char hex[] = "0123456789ABCDEF";
	if (high) ch = (ch >> 4) & 0x0F;
	else ch = ch & 0x0F;
	return hex[ch];
}
unsigned int cfiss_filename_encode(const char *in, char *out, unsigned int size)
{
	unsigned int i = 0, j = 0;
	if (in==NULL) return 0;
	if (out==NULL && size==0) size = 0xFFFFFFFF;
	for (i=0, j=0; in[i]!=0; i++)
	{
		if (is_valid_character(in[i])) // to 1 character
		{
			if (j<(size-1))
			{
				if (out) out[j] = in[i];
				j++;
			}
		}
		else // to 3 characters
		{
			if ((j+2)<(size-1))
			{
				if (out)
				{
					out[j] = '%';
					out[j+1] = nv_to_hex(true, in[i]);
					out[j+2] = nv_to_hex(false, in[i]);
				}
				j += 3;
			}
		}
	}
	if (out) out[j] = 0;
	return j;
}

void cfiss_xml_decoder(char* str)
{
	int i, j;
	char* src = str;
	char* dst = str;
	if (str==NULL) return;
	for (i=0,j=0; src[j]!=0;)
	{
		if (src[j]=='&')
		{
			if      (strncmp(&src[j], "&lt;", 4)==0)   { dst[i] = '<' ; j+=4; }
			else if (strncmp(&src[j], "&gt;", 4)==0)   { dst[i] = '>' ; j+=4; }
			else if (strncmp(&src[j], "&amp;", 5)==0)  { dst[i] = '&' ; j+=5; }
			else if (strncmp(&src[j], "&apos;", 6)==0) { dst[i] = '\''; j+=6; }
			else if (strncmp(&src[j], "&quot;", 6)==0) { dst[i] = '\"'; j+=6; }
			else if (strncmp(&src[j], "&#x0D;", 6)==0) { dst[i] = '\r'; j+=6; }
			else if (strncmp(&src[j], "&#x0A;", 6)==0) { dst[i] = '\n'; j+=6; }
			i++;
		}
		else
		{
			dst[i] = src[j];
			i++; j++;
		}
	}
	dst[i] = 0;
}

char* cfiss_xml_encoder(const char* str)
{
	char* dst;
	const char* src = str;
	int i, j, elen = 1;
	if (str==NULL) return NULL;
	for (i=0; str[i]!=0; i++)
	{
		if      (str[i]=='<')  elen+=4;
		else if (str[i]=='>')  elen+=4;
		else if (str[i]=='&')  elen+=5;
		else if (str[i]=='\'') elen+=6;
		else if (str[i]=='\"') elen+=6;
		else if (str[i]=='\r') elen+=6;
		else if (str[i]=='\n') elen+=6;
		else elen++;
	}
	dst = (char*)malloc(elen*sizeof(char)); 
	if (dst==NULL) return NULL;
	for (i=0,j=0; src[j]!=0; j++)
	{
		if      (src[j]=='<')  { strcpy(&dst[i], "&lt;");   i+=4; }
		else if (src[j]=='>')  { strcpy(&dst[i], "&gt;");   i+=4; }
		else if (src[j]=='&')  { strcpy(&dst[i], "&amp;");  i+=5; }
		else if (src[j]=='\'') { strcpy(&dst[i], "&apos;"); i+=6; }
		else if (src[j]=='\"') { strcpy(&dst[i], "&quot;"); i+=6; }
		else if (src[j]=='\r') { strcpy(&dst[i], "&#x0D;"); i+=6; }
		else if (src[j]=='\n') { strcpy(&dst[i], "&#x0A;"); i+=6; }
		else
		{
			if (i<elen*sizeof(char))
			{ dst[i] = src[j]; }
			i++;
		}
	}
	if (i<elen*sizeof(char)) dst[i] = 0;
	return dst;
}