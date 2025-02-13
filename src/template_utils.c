#include "template_utils.h"

char* replace_placeholders(char* result, const char** keys, const char** values, int num_pairs) {
  for (int i = 0; i < num_pairs; i++) {
    char placeholder[64];
    snprintf(placeholder, sizeof(placeholder), "{{%s}}", keys[i]);
    
    char* position;

    while ((position = strstr(result, placeholder)) != NULL) {
      size_t len_before = position - result;
      size_t len_key = strlen(placeholder);
      size_t len_value = strlen(values[i]);

      char* new_result = malloc(len_before + len_value + strlen(position + len_key) + 1);
      if (!new_result) {
        perror("Memory allocation failed");
        return NULL;
      }

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
    char condition[64];
    snprintf(condition, sizeof(condition), "{%% if %s %%}", keys[i]);

    char* if_position;
    while ((if_position = strstr(result, condition)) != NULL) {
      char* else_position = strstr(if_position, "{% else %}");
      char* endif_position = strstr(if_position, "{% endif %}");
      if (else_position && endif_position) {
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

        char* replacement_content = strlen(values[i]) > 0 ? content_if : content_else;

        size_t len_before = if_position - result;
        size_t len_after = strlen(endif_position + strlen("{% endif %}"));
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
  char loop_start[64];
  snprintf(loop_start, sizeof(loop_start), "{%% for %s in items %%}", loop_key);
  char* loop_position;

  while ((loop_position = strstr(result, loop_start)) != NULL) {
    char* end_loop_position = strstr(loop_position, "{% endfor %}");
    if (!end_loop_position) break;

    size_t len_before = loop_position - result;
    size_t len_after = strlen(end_loop_position + strlen("{% endfor %}"));
    size_t loop_content_length = end_loop_position - (loop_position + strlen(loop_start));

    char* loop_content = strndup(loop_position + strlen(loop_start), loop_content_length);
    if (!loop_content) {
      perror("Memory allocation failed for loop content");
      free(result);
      return NULL;
    }

    char* new_result = malloc(len_before + (loop_content_length + strlen(loop_content)) * loop_count + len_after + 1);
    if (!new_result) {
      perror("Memory allocation failed for new result");
      free(loop_content);
      free(result);
      return NULL;
    }

    strncpy(new_result, result, len_before);

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

    strcpy(new_result + len_before, end_loop_position + strlen("{% endfor %}"));

    free(loop_content);
    free(result); 
    result = new_result;
  }
  return result;
}

char* process_template(const char* template_str, const char** keys, const char** values, int num_pairs, const char* loop_key, const char** loop_values, int loop_count) {
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