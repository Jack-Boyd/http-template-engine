#include "template_utils.h"

char* replace_placeholders(char* result, const char** keys, const char** values, int num_pairs) {
  for (int i = 0; i < num_pairs; i++) {
    //Dynamically create what placeholder keys will look like
    char placeholder[64];
    snprintf(placeholder, sizeof(placeholder), "{{%s}}", keys[i]);
    
    char* position;

    //Anytime the placeholder appears, do this block of code
    while ((position = strstr(result, placeholder)) != NULL) {
      //Calculate how big the new result will be
      size_t len_before = position - result;
      size_t len_key = strlen(placeholder);
      size_t len_value = strlen(values[i]);

      char* new_result = malloc(len_before + len_value + strlen(position + len_key) + 1);
      if (!new_result) {
        perror("Memory allocation failed");
        return NULL;
      }
      //Create new result
      strncpy(new_result, result, len_before);
      strcpy(new_result + len_before, values[i]);
      strcpy(new_result + len_before + len_value, position + len_key);

      free(result); 
      result = new_result;
    }
  }
  return result;
}

char* process_if_else(char* result, const char** keys, const char** values, int num_pairs) {
  for (int i = 0; i < num_pairs; i++) {
    //Create dynamic if statements based on keys
    char condition[64];
    snprintf(condition, sizeof(condition), "{%% if %s %%}", keys[i]);

    //Loop through if statements
    //Only supports if statements with an else conditional, also needs endif to help templating
    char* if_position;
    while ((if_position = strstr(result, condition)) != NULL) {
      char* else_position = strstr(if_position, "{% else %}");
      char* endif_position = strstr(if_position, "{% endif %}");
      if (else_position && endif_position) {
        //Grab HTML content for if and else 
        char* block_start = if_position + strlen(condition);
        char* block_end = else_position;

        char* content_if = strndup(block_start, block_end - block_start);
        char* content_else = strndup(else_position + strlen("{% else %}"), endif_position - (else_position + strlen("{% else %}")));

        if (!content_if || !content_else) {
          perror("Memory allocation failed for if-else content");
          free(content_if);
          free(content_else);
          free(result);
          return NULL;
        }

        //Decide based on keys value what content to render
        char* replacement_content = strlen(values[i]) > 0 ? content_if : content_else;

        size_t len_before = if_position - result;
        size_t len_after = strlen(endif_position + strlen("{% endif %}"));
        
        //Create new result
        char* new_result = malloc(len_before + strlen(replacement_content) + len_after + 1);
        if (!new_result) {
          perror("Memory allocation failed for new result");
          free(content_if);
          free(content_else);
          free(result);
          return NULL;
        }

        strncpy(new_result, result, len_before);
        strcpy(new_result + len_before, replacement_content);
        strcpy(new_result + len_before + strlen(replacement_content), endif_position + strlen("{% endif %}"));

        free(result); 
        result = new_result;

        free(content_if); 
        free(content_else);
      }
      else {
        break;
      }
    }
  }
  return result;
}

char* process_loops(char* result, const char* loop_key, const char** loop_values, int loop_count) {
  //Dynamically create loop key
  char loop_start[64];
  snprintf(loop_start, sizeof(loop_start), "{%% for %s in items %%}", loop_key);
  char* loop_position;

  while ((loop_position = strstr(result, loop_start)) != NULL) {
    //Find entire loop section
    char* end_loop_position = strstr(loop_position, "{% endfor %}");
    if (!end_loop_position) break;

    size_t len_before = loop_position - result;
    size_t len_after = strlen(end_loop_position + strlen("{% endfor %}"));
    size_t loop_content_length = end_loop_position - (loop_position + strlen(loop_start));

    //Create loop content
    char* loop_content = strndup(loop_position + strlen(loop_start), loop_content_length);
    if (!loop_content) {
      perror("Memory allocation failed for loop content");
      free(result);
      return NULL;
    }

    //Create new result with everything before loop
    char* new_result = malloc(len_before + (loop_content_length + strlen(loop_content)) * loop_count + len_after + 1);
    if (!new_result) {
      perror("Memory allocation failed for new result");
      free(loop_content);
      free(result);
      return NULL;
    }

    strncpy(new_result, result, len_before);
    //Go through each item in the loop and replace content dynamically
    for (int i = 0; i < loop_count; i++) {
      char item_placeholder[64];
      snprintf(item_placeholder, sizeof(item_placeholder), "{{%s}}", loop_key);

      char* loop_content_copy = strdup(loop_content);
      if (!loop_content_copy) {
        perror("Failed to duplicate loop content");
        continue;
      }
      char* loop_result = replace_placeholders(loop_content_copy, (const char**)&loop_key, (const char**)&loop_values[i], 1);
      if (loop_result) {
        strcpy(new_result + len_before, loop_result);
        len_before += strlen(loop_result);
        free(loop_result);
      }
    }
    //Add rest of HTML to end
    strcpy(new_result + len_before, end_loop_position + strlen("{% endfor %}"));

    free(loop_content);
    free(result); 
    result = new_result;
  }
  return result;
}

char* process_template(const char* template_str, const char** keys, const char** values, int num_pairs, const char* loop_key, const char** loop_values, int loop_count) {
  //Duplicate string to process template
  char* result = strdup(template_str);
  if (!result) {
    perror("Failed to allocate memory for template processing");
    return NULL;
  }

  result = replace_placeholders(result, keys, values, num_pairs);
  result = process_if_else(result, keys, values, num_pairs);
  result = process_loops(result, loop_key, loop_values, loop_count);

  return result;
}