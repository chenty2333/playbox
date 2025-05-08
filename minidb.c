#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int atoui(const char *str) {
  return (unsigned int)strtoul(str, NULL, 10);
}

struct Address {
  unsigned int id;
  unsigned int set;
  char *name;
  char *email;
};

struct Database {
  unsigned int max_data;
  unsigned int max_rows;
  struct Address *rows;
};

struct Connection {
  FILE *file;
  struct Database *db;
};

void Database_close(struct Connection *conn);

void die(struct Connection *conn, const char *message) {
  if (conn)
    Database_close(conn);
  if (errno) {
    perror(message);
  } else {
    printf("ERROR: %s\n", message);
  }

  exit(1);
}

void Address_print(struct Address *addr) {
  printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn) {
  fread(&conn->db->max_data, sizeof(unsigned int), 1, conn->file);
  fread(&conn->db->max_rows, sizeof(unsigned int), 1, conn->file);

  unsigned int *data = &conn->db->max_data;
  unsigned int *rows = &conn->db->max_rows;

  conn->db->rows = malloc(sizeof(struct Address) * (*rows));
  if (!conn->db->rows)
    die(conn, "Memory allocation for rows failed");

  unsigned int i = 0;
  for (i = 0; i < *rows; i++) {
    struct Address *addr = &conn->db->rows[i];

    addr->name = calloc(*data, sizeof(char));
    addr->email = calloc(*data, sizeof(char));

    if (addr->name == NULL || addr->email == NULL)
      die(conn, "Memory allocation for Address failed.");

    fread(&addr->id, sizeof(unsigned int), 1, conn->file);
    fread(&addr->set, sizeof(unsigned int), 1, conn->file);
    fread(addr->name, *data, 1, conn->file);
    fread(addr->email, *data, 1, conn->file);
  }
}

struct Connection *Database_open(const char *filename, char mode) {
  struct Connection *conn = malloc(sizeof(struct Connection));
  if (!conn)
    die(conn, "Memory error!");

  conn->db = malloc(sizeof(struct Database));
  if (!conn->db)
    die(conn, "Memory error!");

  if (mode == 'c') {
    conn->file = fopen(filename, "w");
  } else {
    conn->file = fopen(filename, "r+");

    if (conn->file) {
      Database_load(conn);
    }
  }

  if (!conn->file)
    die(conn, "Failed to open the file.");

  return conn;
}

void Database_close(struct Connection *conn) {
  if (conn) {
    if (conn->file)
      fclose(conn->file);
    if (conn->db) {
      unsigned int i = 0;
      for (i = 0; i < conn->db->max_rows; i++) {
        free(conn->db->rows[i].name);
        free(conn->db->rows[i].email);
      }

      free(conn->db->rows);
      free(conn->db);
    }

    free(conn);
  }
}

void Database_write(struct Connection *conn) {
  rewind(conn->file);

  long int rc =
      fwrite(&conn->db->max_data, sizeof(unsigned int), 1, conn->file);
  if (rc != 1)
    die(conn, "Failed to write database.");

  rc = fwrite(&conn->db->max_rows, sizeof(unsigned int), 1, conn->file);
  if (rc != 1)
    die(conn, "Failed to write database.");

  unsigned int *data = &conn->db->max_data;
  unsigned int *rows = &conn->db->max_rows;

  unsigned int i = 0;
  for (i = 0; i < *rows; i++) {
    struct Address *addr = &conn->db->rows[i];

    if (addr->name == NULL || addr->email == NULL)
      die(conn, "Memory allocation for Address failed.");

    fwrite(&addr->id, sizeof(unsigned int), 1, conn->file);
    fwrite(&addr->set, sizeof(unsigned int), 1, conn->file);
    fwrite(addr->name, *data, 1, conn->file);
    fwrite(addr->email, *data, 1, conn->file);
  }

  rc = fflush(conn->file);
  if (rc == -1)
    die(conn, "Cannot flush database.");
}

void Database_create(struct Connection *conn, unsigned int max_data,
                     unsigned int max_rows) {
  unsigned int i = 0;

  conn->db->max_data = max_data;
  conn->db->max_rows = max_rows;

  conn->db->rows = malloc(sizeof(struct Address) * max_rows);
  if (!conn->db->rows)
    die(conn, "Memory allocation for rows failed in create.");

  for (i = 0; i < max_rows; i++) {
    conn->db->rows[i].id = i;
    conn->db->rows[i].set = 0;
    conn->db->rows[i].name = calloc(max_data, sizeof(char));
    conn->db->rows[i].email = calloc(max_data, sizeof(char));
  }
}

void Database_set(struct Connection *conn, unsigned int id, const char *name,
                  const char *email) {
  struct Address *addr = &conn->db->rows[id];
  if (addr->set)
    die(conn, "Already set, delete it first.");

  addr->set = 1;
  strncpy(addr->name, name, conn->db->max_data);
  // if(!res) die("Name copy failed.");
  addr->name[conn->db->max_data - 1] = '\0';

  strncpy(addr->email, email, conn->db->max_data);
  // if(!res) die("Email copy failed");
  addr->email[conn->db->max_data - 1] = '\0';
}

void Database_get(struct Connection *conn, unsigned int id) {
  struct Address *addr = &conn->db->rows[id];

  if (addr->set) {
    Address_print(addr);
  } else {
    die(conn, "ID is not set.");
  }
}

void Database_delete(struct Connection *conn, unsigned int id) {
  struct Address *addr = &conn->db->rows[id];
  free(addr->name);
  free(addr->email);

  addr->id = id;
  addr->set = 0;
  addr->name = calloc(conn->db->max_data, sizeof(char));
  addr->email = calloc(conn->db->max_data, sizeof(char));

  if (!addr->name || !addr->email)
    die(conn, "Memory allocation failed in delete");
}

void Database_list(struct Connection *conn) {
  unsigned int i = 0;
  struct Database *db = conn->db;

  for (i = 0; i < conn->db->max_rows; i++) {
    struct Address *cur = &db->rows[i];

    if (cur->set) {
      Address_print(cur);
    }
  }
}

void find_name(struct Connection *conn, const char *name) {
  unsigned int i = 0;
  int found = 1;
  for (i = 0; i < conn->db->max_rows; i++) {
    if (conn->db->rows[i].set) {
      found = strcmp(name, conn->db->rows[i].name);

      struct Address *addr = &conn->db->rows[i];
      if (!found) {
        Address_print(addr);
        break;
      }
    }
  }
  if (i == conn->db->max_rows)
    die(conn, "The name is not found.");
}

unsigned int check_id(struct Connection *conn, int argc, char *argv[]) {
  if (argc < 4)
    die(conn, "Missing id");

  unsigned int id = atoui(argv[3]);

  if (id >= conn->db->max_rows)
    die(conn, "There's not that many records.");

  return id;
}

int main(int argc, char *argv[]) {
  if (argc < 3)
    die(NULL, "USAGE: ex17 <dbfile> <action> [action params]");

  char *filename = argv[1];
  char action = argv[2][0];
  struct Connection *conn = Database_open(filename, action);
  unsigned int id = 0;

  switch (action) {
  case 'c':
    if (argc != 5)
      die(NULL, "Need max_data and max_rows");
    unsigned max_data = atoui(argv[3]);
    unsigned max_rows = atoui(argv[4]);

    Database_create(conn, max_data, max_rows);
    Database_write(conn);
    break;

  case 'g':
    id = check_id(conn, argc, argv);
    if (argc != 4)
      die(conn, "Need an id to get");

    Database_get(conn, id);
    break;

  case 's':
    id = check_id(conn, argc, argv);
    if (argc != 6)
      die(conn, "Need id, name, email to set");

    Database_set(conn, id, argv[4], argv[5]);
    Database_write(conn);
    break;

  case 'd':
    id = check_id(conn, argc, argv);
    if (argc != 4)
      die(conn, "Need id to delete");

    Database_delete(conn, id);
    Database_write(conn);
    break;

  case 'l':
    Database_list(conn);
    break;

  case 'f':
    if (argc < 4)
      die(conn, "Need a name to find");
    find_name(conn, argv[3]);
    break;

  default:
    die(conn, "Invalid action, only: c=create, g=get, s=set, d=del, l=list");
  }

  Database_close(conn);

  return 0;
}
