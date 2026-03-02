#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "instruction.h"
#include "opcodes.h"
int parse_register(const char *str)
{
  while(*str == ' ' || *str == '\t')str++;
  char clean[16];
  strncpy(clean,str,15);
  clean[15] = '\0';
  char *comma = strchr(clean,',');
  if(comma) *comma = '\0';
  int len = strlen(clean);
  while(len > 0 && clean[len - 1] <= ' ')clean[--len] = '\0';
  if((clean[0] == 'r' || clean[0] == 'R') && (clean[1] >= '0' && clean[1] <= '7') && (!clean[2]))
  {
    return clean[1]-'0';
  }
  return -1;
}
int parse_immediate(const char *str)
{
  while(*str == ' ' || *str == '\t')str++;
  return atoi(str);
}
typedef struct{
char name[32];
uint16_t address;
}Label;
int assemble(const char *source,uint16_t *output,int max_words)
{
  Label labels[256];
  int label_count = 0;
  int word_count = 0;
  char *src = malloc(strlen(source)+1);
  strcpy(src,source);
  char *line = strtok(src,"\n");
  uint16_t addr = 0;
  while(line)
  {
    char *t = line;
    while(*t == ' ' || *t == '\t')t++;
    if(*t && *t != ';')
    {
      char *colon = strchr(t,':');
      if(colon && colon[1] == '\0')
      {
        *colon = '\0';
        strncpy(labels[label_count].name,t,31);
        labels[label_count].name[31] = '\0';
        labels[label_count].address = addr;
        label_count++;
      }
      else
      {
        char mn[16];
        sscanf(t,"%15s",mn);
        for(int i = 0 ; mn[i] ; i++)
        {
         mn[i] = toupper(mn[i]);
        }
        if(!strcmp(mn,"LOAD") || !strcmp(mn,"CALL"))
        {
          addr += 4;
        }
        else
        {
          addr +=2;
        }
      }
    }
    line = strtok(NULL,"\n");
  }
  free(src);
  src = malloc(strlen(source) + 1);
  strcpy(src,source);
  line = strtok(src,"\n");
  while(line && word_count < max_words)
  {
    char *t = line;
    while(*t == ' ' || *t == '\t')t++;
    if(!*t || *t == ';' || strchr(t,':'))
    {
      line = strtok(NULL,"\n");
      continue;
    }
    char mn[16],a1[32]="",a2[32]="";
    sscanf(t,"%15s %31[^,],%31s",mn,a1,a2);
    for(int i = 0 ; mn[i] ; i++)
    {
      mn[i] = toupper(mn[i]);
    }
    if(!strcmp(mn,"NOP"))output[word_count++] = ENCODE_REG(OP_NOP,0,0,0);
    else if(!strcmp(mn,"HALT"))output[word_count++] = ENCODE_REG(OP_HALT,0,0,0);
    else if(!strcmp(mn,"RET"))output[word_count++] = ENCODE_REG(OP_RET,0,0,0);
    else if(!strcmp(mn,"ADD"))output[word_count++] = ENCODE_REG(OP_ADD,parse_register(a1),parse_register(a2),0);
    else if(!strcmp(mn,"MOV"))output[word_count++] = ENCODE_REG(OP_MOV,parse_register(a1),parse_register(a2),0);
    else if(!strcmp(mn,"SUB"))output[word_count++] = ENCODE_REG(OP_SUB,parse_register(a1),parse_register(a2),0);
    else if(!strcmp(mn,"AND"))output[word_count++] = ENCODE_REG(OP_AND,parse_register(a1),parse_register(a2),0);
    else if(!strcmp(mn,"OR"))output[word_count++] = ENCODE_REG(OP_OR,parse_register(a1),parse_register(a2),0);
    else if(!strcmp(mn,"XOR"))output[word_count++] = ENCODE_REG(OP_XOR,parse_register(a1),parse_register(a2),0);
    else if(!strcmp(mn,"NOT"))output[word_count++] = ENCODE_REG(OP_NOT,parse_register(a1),0,0);
    else if(!strcmp(mn,"CMP"))output[word_count++] = ENCODE_REG(OP_CMP,parse_register(a1),parse_register(a2),0);
    else if(!strcmp(mn,"LDR"))output[word_count++] = ENCODE_REG(OP_LDR,parse_register(a1),parse_register(a2),0);
    else if(!strcmp(mn,"STR"))output[word_count++] = ENCODE_REG(OP_STR,parse_register(a1),parse_register(a2),0);
    else if(!strcmp(mn,"SHL"))output[word_count++] = ENCODE_REG(OP_SHL,parse_register(a1),0,parse_immediate(a2));
    else if(!strcmp(mn,"SHR"))output[word_count++] = ENCODE_REG(OP_SHR,parse_register(a1),0,parse_immediate(a2));
    else if(!strcmp(mn,"PUSH"))output[word_count++] = ENCODE_REG(OP_PUSH,parse_register(a1),0,0);
    else if(!strcmp(mn,"POP"))output[word_count++] = ENCODE_REG(OP_POP,parse_register(a1),0,0);
    else if(!strcmp(mn,"LOAD"))
    {
      output[word_count++] = ENCODE_REG(OP_LOAD,parse_register(a1),0,0);
      output[word_count++] = (uint16_t)parse_immediate(a2);
    }
    else if(!strcmp(mn,"CALL"))
    {
      uint16_t target = 0;
      for(int i = 0 ; i < label_count ; i++)
      {
        if(!strcmp(a1,labels[i].name))
        {
          target = labels[i].address;
          break;
        }
      }
      output[word_count++] = ENCODE_REG(OP_CALL,0,0,0);
      output[word_count++] = target;
    }
    else if(!strcmp(mn,"JMP") || !strcmp(mn,"JZ") || !strcmp(mn,"JNZ") ||  !strcmp(mn,"JN"))
    {
      Opcode jump = !strcmp(mn,"JMP") ? OP_JMP : !strcmp(mn,"JZ") ? OP_JZ : !strcmp(mn,"JNZ") ? OP_JNZ : OP_JN;
      uint16_t target = 0;
      int found = 0;
      for(int i = 0 ; i < label_count ; i++)
      {
        if(!strcmp(a1,labels[i].name))
        {
          target = labels[i].address;
          found = 1;
          break;
        }
      }
      if(!found && a1[0])target = atoi(a1);
      output[word_count++] = ENCODE_JUMP(jump,target);
    }
    else
    {
      fprintf(stderr,"Unknown instruction : %s\n",mn);
    }
    line = strtok(NULL,"\n");
  }
  free(src);
  return word_count;
}
