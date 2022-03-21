#pragma once
#include "logger.h"
#include "stdlib.h"

void red() { printf("\033[31m"); }
void yellow() { printf("\033[33m"); }
void cyan() { printf("\033[36m"); }

void bold() { printf("\033[1m"); }
void reset() { printf("\033[0m"); }

void print_fatal(char *msg) {
  bold();
  red();
  printf("FATAL: ");
  reset();
  printf("%s\n", msg);
  exit(1);
}

void print_warn(char *msg) {
  bold();
  yellow();
  printf("WARNING: ");
  reset();
  printf("%s\n", msg);
}

void print_info(char *msg) {
  bold();
  cyan();
  printf("INFO: ");
  reset();
  printf("%s\n", msg);
}
