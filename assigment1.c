//gcc assigment1.c -o assigment1  -I/usr/include/libxml2 -lxml2 -ljson
//./assigment1 xmlInput.xml

#include "stdio.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "split.h"
#include "json/json.h"

char *xmlName = NULL;
char *jsonName = NULL;
int same = 0;

xmlDocPtr jsonToXML_DOC = NULL;
xmlNodePtr jsonToXML_root = NULL;
json_object *JSONroot_object = NULL;


void jsonParse(json_object *jobj, xmlNodePtr xmlRoot) {
  json_object *firstJobj;
  xmlNodePtr newNode;
  enum json_type type;

  json_object_object_foreach(jobj, key, val) {
    type = json_object_get_type(val);
    switch (type) {
      case json_type_array:

      newNode = xmlNewChild(xmlRoot, NULL, BAD_CAST key, NULL);
      json_object *jsonArray = json_object_new_object();
      if(key){
        jsonArray = json_object_object_get(jobj, key);
      }

      int arrayLength = json_object_array_length(jsonArray);
      int i;
      json_object *arrayObjects = json_object_new_object();

      for (i = 0; i < arrayLength; i++) {
        arrayObjects = json_object_array_get_idx(jsonArray, i);
        printf("%s\n", json_object_get_string(val));
        json_object_object_foreach(arrayObjects, key, val){

          printf("%s\n", key);
          printf("%s\n", json_object_get_string(val));
          xmlNewChild(newNode, NULL, BAD_CAST key, json_object_get_string(val));
        }
      }
      break;

      case json_type_object:
      firstJobj = json_object_object_get(jobj,key);
      newNode = xmlNewChild(xmlRoot, NULL, BAD_CAST key, NULL);
      jsonParse(firstJobj,newNode);
      break;

      default:
      xmlNewChild(xmlRoot, NULL, BAD_CAST key, BAD_CAST json_object_get_string(val));
      break;
    }
  }
}

void jsonToXML(char *parsedInput,json_object *json){
  jsonToXML_DOC = xmlNewDoc(BAD_CAST "1.0");
  jsonToXML_root = xmlNewNode(NULL, BAD_CAST"root");
  xmlDocSetRootElement(jsonToXML_DOC,jsonToXML_root);

  jsonParse(json,jsonToXML_root);

  htmlSaveFileEnc(strcat(strcat(parsedInput,"Output"),".xml"), jsonToXML_DOC, "UTF-­8", 1);
}

//XML -> JSON BELOW

int isChild(xmlNode *currentNode){
  for (currentNode = currentNode->children; currentNode; currentNode = currentNode->next) {
    if (currentNode->type == XML_ELEMENT_NODE) {
      return 0;
    }
  }
  return 1;
}

void xmlToJSON(xmlNode *a_node, json_object *jsonRoot){
  xmlNode *currentNode = NULL;     //node
  xmlAttr *NodeAttribute = NULL;   //attribute as an object
  xmlChar *attribute;              //char or char array
  json_object *newObj;
  json_object *newArray;

  char *lastElementName = malloc(100);

  for (currentNode = a_node; currentNode; currentNode = currentNode->next) {
    if (currentNode->type == XML_ELEMENT_NODE) {
      if(isChild(currentNode)){
        newObj = json_object_new_string(currentNode->children->content);
        xmlNode *nextNode = currentNode->next;

        while(nextNode && nextNode->type != XML_ELEMENT_NODE){
          nextNode = nextNode->next;
        }

        if(nextNode && strcmp(currentNode->name,nextNode->name) == 0){
          if(same == 0){
            newArray = json_object_new_array();
            json_object_object_add(jsonRoot,currentNode->name,newArray);
            strcpy(lastElementName, currentNode->name);
          }
          same = 1;
          json_object_array_add(newArray,newObj);
          continue;
        }

        else{
          if(strcmp(lastElementName, currentNode->name) == 0){

            json_object_array_add(newArray,newObj);
          }
          else{
            json_object_object_add(jsonRoot,currentNode->name,newObj);
          }
        }
      }

      else{
        same = 0;
        newObj = json_object_new_object();
        json_object_object_add(jsonRoot,currentNode->name,newObj);
        xmlToJSON(currentNode->children,newObj);
      }
    }
  }
}

void readXML(char *xmlName, json_object *jsonRoot){
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;
  char *Filename = xmlName;
  doc = xmlReadFile(Filename, NULL, 0);

  if(doc == NULL){
    printf("Error: couldn't parse file%s\n", Filename);
  }
  else{
    root_element = xmlDocGetRootElement(doc);
    xmlToJSON(root_element,jsonRoot);
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
    readXML(xmlName,JSONroot_object);
    json_object_to_file(strcat(strcat(parsedInput[1],"Output"),".json"), JSONroot_object);
    free(xmlName);
  }
  else if(strcmp(parsedInput[1],"json") == 0){
    jsonName = malloc(strlen(argv[1])+1);
    strcpy(jsonName, argv[1]);
    json_object *mainJSON_Object = json_object_from_file(jsonName);
    jsonToXML(parsedInput[1],mainJSON_Object);
  }

}

int main(int argc, char *argv[]) {
  operations(argc,argv);


  return 0;
}
