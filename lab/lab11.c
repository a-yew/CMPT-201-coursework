#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED "\e[9;31m"
#define GRN "\e[0;32m"
#define CRESET "\e[0m"

#define handle_error(msg)                                                                          \
  do {                                                                                             \
    perror(msg);                                                                                   \
    exit(EXIT_FAILURE);                                                                            \
  } while (0)

size_t read_all_bytes(const char *filename, void *buffer, size_t buffer_size) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    handle_error("Error opening file");
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (file_size > buffer_size) {
    handle_error("File size is too large");
  }

  if (fread(buffer, 1, file_size, file) != file_size) {
    handle_error("Error reading file");
  }

  fclose(file);
  return file_size;
}

void print_file(const char *filename, const char *color) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    handle_error("Error opening file");
  }

  printf("%s", color);
  char line[256];
  while (fgets(line, sizeof(line), file)) {
    printf("%s", line);
  }
  fclose(file);
  printf(CRESET);
}

int verify(const char *message_path, const char *sign_path, EVP_PKEY *pubkey);

int main() {
  // File paths
  const char *message_files[] = {"message1.txt", "message2.txt", "message3.txt"};
  const char *signature_files[] = {"signature1.sig", "signature2.sig", "signature3.sig"};

  // TODO: Load the public key using PEM_read_PUBKEY
  // EVP_PKEY *pubkey = NULL;
  FILE *pubkey_file = fopen("public_key.pem", "r");
  EVP_PKEY *pubkey = PEM_read_PUBKEY(pubkey_file, NULL, NULL, NULL);
  fclose(pubkey_file);

  // Verify each message
  for (int i = 0; i < 3; i++) {
    printf("... Verifying message %d ...\n", i + 1);
    int result = verify(message_files[i], signature_files[i], pubkey);

    if (result < 0) {
      printf("Unknown authenticity of message %d\n", i + 1);
      print_file(message_files[i], CRESET);
    } else if (result == 0) {
      printf("Do not trust message %d!\n", i + 1);
      print_file(message_files[i], RED);
    } else {
      printf("Message %d is authentic!\n", i + 1);
      print_file(message_files[i], GRN);
    }
  }

  EVP_PKEY_free(pubkey);

  return 0;
}

/*
    Verify that the file `message_path` matches the signature `sign_path`
    using `pubkey`.
    Returns:
         1: Message matches signature
         0: Signature did not verify successfully
        -1: Message is does not match signature
*/
int verify(const char *message_path, const char *sign_path, EVP_PKEY *pubkey) {
#define MAX_FILE_SIZE 512
  unsigned char message[MAX_FILE_SIZE];
  unsigned char signature[MAX_FILE_SIZE];

  // TODO: Check if the message is authentic using the signature.
  // Look at: https://wiki.openssl.org/index.php/EVP_Signing_and_Verifying
  size_t msg_len = read_all_bytes(message_path, message, MAX_FILE_SIZE);
  size_t sig_len = read_all_bytes(sign_path, signature, MAX_FILE_SIZE);

  EVP_MD_CTX *mdctx = EVP_MD_CTX_new();

  EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pubkey);
  EVP_DigestVerifyUpdate(mdctx, message, msg_len);
  int result = EVP_DigestVerifyFinal(mdctx, signature, sig_len);
  EVP_MD_CTX_free(mdctx);

  if (result == 1) {
    // 1: Message matches signature
    return 1;
  } else if (result == 0) {
    // -1: Message is does not match signature
    return -1;
  } else {
    // 0: Signature did not verify successfully
    return 0;
  }

  return -1;
}
