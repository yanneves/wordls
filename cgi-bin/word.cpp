#include <stdio.h>
#include <malloc.h>
#include <stddef.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <regex.h>
#include <vector>
#include <algorithm>

using namespace std;

const int MAX_LINE_LEN=15;

const char *dict = "sowpods.txt";

inline void wipe(char c,char *str,int len) {
  int i;
  for (i=0; i<len; i++) {
    if (str[i] == c) {
      str[i] = ' ';
      break;
    }
  }
}

inline void wipeblank(char *str,int len) {
  int i;
  for (i=0; i<len; i++) {
    if (str[i] != ' ') {
      str[i] = ' ';
      break;
    }
  }
}

inline bool isempty(char *str,int len) {
  int i;
  for (i=0; i<len; i++) {
    if (str[i] != ' ') {
      return false;
    }
  }
  return true;
}

typedef struct {
  char word[MAX_LINE_LEN+1];
  int len;
} match_t;

static bool match_t_cmp(const match_t& m1, const match_t& m2)
{
  if (m1.len == m2.len)
    return (strcmp(m1.word, m2.word) < 0);
  return (m1.len > m2.len);
}

int main(int argc,char**argv)
{
  if (argc < 2 || argc > 3) {
    cout << "Usage: " << argv[0] << "<rack> [<pattern>]" << endl;
    return 1;
  }

  const char *in_rack = argv[1];
  const int in_rack_len = strlen(in_rack);
  const char *in_patt = (argc == 3 ? argv[2] : NULL);
  const int in_patt_len = (in_patt ? strlen(in_patt) : 0);

  char *rack = (char*)malloc(in_rack_len+1);
  char *patt = (char*)malloc(in_patt_len+1);
  int min_count = 0, max_count = 0; // Count of min/max chars in a matched word

  // Remove any unrecognised chars from <rack> and convert
  int i;
  char *s = rack;
  for (i=0; i<in_rack_len; i++) {
    const char c = in_rack[i];
    if (isalpha(c)) { *s++ = toupper(c); max_count++; }
    else if (c == '?') { *s++ = c; max_count++; }
  }
  *s = '\0';
  if (s == rack) {
    cout << "<rack> string is empty!" << endl;
    return 1;
  }
  int rack_len = s-rack;

  s = patt;
  // Remove any unrecognised chars from <patt> and convert '?' into '.'
  // Only allow any '$' chars at the beginning or end of pattern signified with beg/end
  bool beg = false, end = false;
  bool range = false; // signifies we're in a [...] range
  for (i=0; i<in_patt_len; i++) {
    const char c = in_patt[i];
    if (isalpha(c)) { *s++ = toupper(c); if (!range) { min_count++; max_count++; } end = false; }
    else if (c == '?' || c == '.') { *s++ = '.'; min_count++; end = false; }
    else if (c == '$') { if (s == patt) beg = true; else end = true; }
    else if (c == '[') { *s++ = c; range = true; }
    else if (c == ']') { *s++ = c; min_count++; max_count++; range = false; }
  }
  if (end)
    *s++ = '$';
  *s = '\0';
  int patt_len = s-patt;
  if (beg) {
    memmove(&patt[1],&patt[0],s-patt+1);
    patt[0] = '^';
    patt_len++;
  }

//cout << rack << " " << patt << " min: " << min_count << " max: " << max_count << endl;

  ifstream fin(dict);
  if (!fin.is_open()) {
    cout << "Could not open dictonary file: " << dict << endl;
    return 1;
  }
  int ret=0;
  regex_t preg;
  if ((ret=regcomp(&preg,patt,REG_EXTENDED|REG_NOSUB))) {
    cout << "Problem with regcomp: " << ret << endl;
    return 1;
  }
  vector<match_t> matches;
  match_t match;
  char scratch[MAX_LINE_LEN+1];
  while (fin.getline(match.word,sizeof(match.word))) {
    if (regexec(&preg,match.word,0,NULL,0) == 0) {
      match.len = strlen(match.word);
      if (match.len < min_count || match.len > max_count)
        continue;
      strcpy(scratch,match.word);
      bool range = false;
      for (i=0; i<patt_len; i++) {
        const char c = patt[i];
        if (isalpha(c)) { if (!range) wipe(c,scratch,match.len); }
        else if (c == '[') { range = true; }
        else if (c == ']') { range = false; }
      }
      for (i=0; i<rack_len; i++) {
        const char c = rack[i];
        if (isalpha(c)) { wipe(c,scratch,match.len); }
      }
      for (i=0; i<rack_len; i++) {
        const char c = rack[i];
        if (c == '?') { wipeblank(scratch,match.len); }
      }
      if (!isempty(scratch,match.len))
        continue;

      // Line matches
      matches.push_back(match);
    }
  }
  regfree(&preg);

  fin.close();

  free(rack);
  free(patt);

  sort(matches.begin(), matches.end(), match_t_cmp);
  for (vector<match_t>::const_iterator itr = matches.begin(); itr != matches.end(); itr++) {
    cout << (*itr).word << endl;
  }

  return 0;
}
