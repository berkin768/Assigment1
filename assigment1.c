//gcc assigment1.c -o assigment1  -I/usr/include/libxml2 -lxml2 -ljson
//./assigment1 test.xml

#include "stdio.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "split.h"
#include "json/json.h"


struct jsonObject{
  char *key;
  char *value;
};

struct xmlObject{
  char *tag;
  char *value;
  char *attribute;
};

struct nodes{
  xmlNodePtr node;
};

struct jsonObject JSON;
struct xmlObject XML;
struct nodes xmlNodes[50];


char *xmlName = NULL;
char *jsonName = NULL;

int count = 0;
int parentNodeCount = 0;
int childNodeCount = 0;
xmlDocPtr jsonToXML = NULL;
xmlNodePtr jsonToXML_root = NULL;

json_object *JSONroot_object;

void createXML(char *key, char const *value, char *type){
  if(count == 0){
    jsonToXML = xmlNewDoc(BAD_CAST "1.0");
    jsonToXML_root = xmlNewNode(NULL, BAD_CAST"root");
    xmlDocSetRootElement(jsonToXML,jsonToXML_root);
  }

  htmlSaveFileEnc("abc.xml", jsonToXML, "UTF-­8", 1);


}

void json_parse(json_object * jobj) {
  char *typeName = malloc(100);
  enum json_type type;
  json_object_object_foreach(jobj, key, val) {

    type = json_object_get_type(val);

    switch (type) {
      case json_type_string:
      printf("type: json_type_string, %s ", key);
      printf("%s\n", json_object_get_string(val));
      strcpy(typeName,"string");
      createXML(key,json_object_get_string(val),typeName);

      break;

      case json_type_int:
      printf("type: json_type_int, %s ", key);
      printf("%d\n", json_object_get_int(val));
      strcpy(typeName,"int");
      createXML(key,json_object_get_string(val),typeName);

      break;

      case json_type_double:
      printf("type: json_type_double, %s ", key);
      printf("%lf\n", json_object_get_double(val));
      strcpy(typeName,"double");
      createXML(key,json_object_get_string(val),typeName);

      break;

      case json_type_boolean:
      printf("type: json_type_boolean, %s ", key);
      if(val == 0){
        printf("%s\n", "false");
      }
      else{
        printf("%s\n", "true");
      }
      strcpy(typeName,"boolean");
      createXML(key,json_object_get_string(val),typeName);

      break;

      case json_type_array:
      printf(" %s\n ", "hi" );
      strcpy(typeName,"array");
      createXML(key,json_object_get_string(val),typeName);

      break;

      case json_type_object:
      printf("type: json_type_object, %s ", key);
      printf("%s\n", json_object_get_string(val));
      strcpy(typeName,"object");
      createXML(key,NULL,typeName);
      json_parse(val);


      break;
    }
  }
}

char *readJSON(char *jsonName){
  FILE *json_file;
  size_t size = 0;
  json_file = fopen(jsonName,"r"); // read mode

  char *json_string;

  fseek(json_file, 0, SEEK_END);
  size = ftell(json_file);

  rewind(json_file);

  json_string = malloc((size + 1) * sizeof(*json_string));

  fread(json_string, size, 1, json_file);

  json_string[size] = '\0';

  return json_string;
}

void xmlWalk(xmlNode *a_node){

  xmlNode *currentNode = NULL;     //node
  xmlAttr *NodeAttribute = NULL;   //attribute as an object
  xmlChar *attribute;              //char or char array

  for (currentNode = a_node; currentNode; currentNode = currentNode->next) {
    if (currentNode->type == XML_ELEMENT_NODE) {
      printf("%s\n", currentNode->name);

      XML.tag = malloc(strlen(currentNode->name)+1);
      strcpy(XML.tag, currentNode->name);

      XML.value = malloc(strlen(currentNode->children->content)+1);
      strcpy(XML.value, currentNode->children->content);

      // if(currentNode->children->next != NULL){
      //   for (currentNode = a _node->children; currentNode; currentNode = currentNode->children) {
      //     //printf("%s\n", currentNode->name);
      //     printf("%s\n", currentNode->children->next->name);
      //
      //   }
      //
      //
      //   json_object *jarray = json_object_new_array();
      //
      // }

      if(strstr(currentNode->children->content, "\n") == 0){
        // json_object *root = XML.tag;
        json_object *value = json_object_new_string(XML.value);
        json_object_object_add(JSONroot_object,XML.tag,value);
      }

    }
    xmlWalk(currentNode->children);
  }
}

void parseXML(char *xmlName){
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;
  char *Filename = xmlName;
  doc = xmlReadFile(Filename, NULL, 0);

  if(doc == NULL){
    printf("Error: couldn't parse file%s\n", Filename);
  }
  else{
    root_element = xmlDocGetRootElement(doc);
    xmlWalk(root_element);
    xmlFreeDoc(doc);
  }
  xmlCleanupParser();
}

void operations(int argc, char *argv[]){
  int i = 0;
  char **parsedInput = malloc(100);

  parsing(parsedInput,argv[1],".");

  if(strcmp(parsedInput[1],"xml") == 0){
    xmlName = malloc(strlen(argv[1])+1);
    strcpy(xmlName, argv[1]);
    JSONroot_object = json_object_new_object();
    parseXML(xmlName);
    printf ("%s\n",json_object_to_json_string(JSONroot_object));
    free(xmlName);
  }
  else if(strcmp(parsedInput[1],"json") == 0){
    jsonName = malloc(strlen(argv[1])+1);
    strcpy(jsonName, argv[1]);
    char *json_string = readJSON(jsonName);
    json_object * jobj = json_tokener_parse(json_string);
    json_parse(jobj);
    // json_object * jobj = json_tokener_parse(jsonName);
    // json_parse(jobj);
  }

}

int main(int argc, char *argv[]) {
  operations(argc,argv);


  return 0;
}
