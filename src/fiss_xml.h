#ifndef _CFISS_XML_H_
#define _CFISS_XML_H_

// FISS object
typedef struct
{
	void		*io;		// FILE IO pointer
	char		*xmlData;	// all XML content
	char		*Data;		// FISS Data content pointer
	long		eOffset;	// last offset filled zero byte
	const char	*errMsg;	// error message
}
CFISS;

//================================================================================================================
// API

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Load

void cfiss_beginLoad(CFISS* obj, const char* filename);
const char* cfiss_loadString(CFISS* obj, const char* name);
bool cfiss_loadBool(CFISS* obj, const char* name);
float cfiss_loadFloat(CFISS* obj, const char* name);
int cfiss_loadInt(CFISS* obj, const char* name);
const char* cfiss_endLoad(CFISS* obj);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save

void cfiss_beginSave(CFISS* obj, const char* filename, const char* modname);
void cfiss_saveBool(CFISS* obj, const char* name, bool value);
void cfiss_saveString(CFISS* obj, const char* name, const char* value);
void cfiss_saveFloat(CFISS* obj, const char* name, float value);
void cfiss_saveInt(CFISS* obj, const char* name, int value);
const char* cfiss_endSave(CFISS* obj);
const char* cfiss_saveTextToTxtFile(const char* filename, const char* text);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Special

void cfiss_xml_decoder(char* str);
char* cfiss_xml_encoder(const char* str);
unsigned int cfiss_filename_encode(const char *in, char *out, unsigned int size);

#ifdef __cplusplus
}
#endif 
#endif // _CFISS_XML_H_
