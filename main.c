#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMBER_ERR fputs ("Por favor, digite um número válido.\n", stderr)

signed menu (void);

typedef
struct Product
{
  char* name;
  size_t amount;
}
Product;

static
Product prod[0x1000];

static
size_t prod_amount;

signed update_file ()
{
  FILE* f = fopen ("estoque.txt", "w");
  if (f == NULL) return 1;

  for (size_t i = 0; i < prod_amount; i ++)
    if (prod[i].amount != 0)
      fprintf (f, "%s\n%lu\n", prod[i].name, prod[i].amount);

  fclose (f);
  return 0;
}

signed read_file ()
{
  FILE* f = fopen ("estoque.txt", "r");
  if (f == NULL) return 1;

  char* name = malloc (sizeof(char) * 0x40);
  size_t amount;

  prod_amount = 0;

  while (fscanf(f, "%s\n%lu\n", name, &amount) == 2)
  {
    prod[prod_amount] = (Product) {name, amount};
    prod_amount ++;
    name = malloc (sizeof(char) * 0x40);
  }

  free(name);

  fclose (f);
  return 0;
}


signed add_item (const Product p)
{
  for (size_t i = 0; i < prod_amount; i ++)
    if (!strcmp(prod[i].name, p.name))
    {
      prod[i].amount += p.amount;
      return 0;
    }

  prod[prod_amount].name = p.name;
  prod[prod_amount].amount = p.amount;
  prod_amount ++;

  return 0;
}

long long remove_item (const Product p)
{
  for (size_t i = 0; i < prod_amount; i ++)
    if (!strcmp(prod[i].name, p.name))
    {
      if (prod[i].amount < p.amount) return prod[i].amount;
      else
      {
        prod[i].amount -= p.amount;
        if (prod[i].amount == 0) return -2;
        return -1;
      }
    }

  return -3;
}

signed menu_continue ()
{
  char c;

  printf ("Deseja realizar outra operação? (s/n): ");

  scanf (" %c", &c);

  switch (c)
  {
  case 'S':
  case 's':
    return menu ();

  case 'N':
  case 'n':
    return 0;

  default:
    fputs ("Por favor, somente digite 's' ou 'n'.\n", stderr);
  }

  return menu_continue ();
}

signed do_op (const uint8_t op)
{
  char* name;
  size_t amount;

  if (op - 1 > 3)
  { fputs ("Opção inválida! Tente novamente.\n", stderr); return menu (); }

  switch (op)
  {
  case 1:
    name = malloc (sizeof(char) * 0x40);

    printf ("Digite o nome do item: ");
    scanf ("%s", name);
    printf ("Digite a quantidade: ");
    scanf ("%lu", &amount);

    if (add_item ((Product) {name, amount}) == 0)
      fputs ("Item adicionado com sucesso!\n", stderr);
    else
      fputs ("Não foi possível adicionar o ítem.\n", stderr);

    break;

  case 2:
    name = malloc (sizeof(char) * 0x40);

    printf ("Digite o nome do item: ");
    scanf ("%s", name);
    printf ("Digite a quantidade: ");
    scanf ("%lu", &amount);

    long long r = remove_item ((Product) {name, amount});
    if (r == -1)
      fputs ("Quantidade atualizada com sucesso!\n", stderr);
    else if (r == -2)
      fputs ("Item removido do estoque!\n", stderr);
    else if (r == -3)
      fputs ("Item não encontrado.\n", stderr);
    else if (r >= 0)
      fprintf (stderr, "Estoque insuficiente. Quantidade disponível: %lld\n", r);
    else
      fputs ("Não foi possível remover o ítem.\n", stderr);

    break;

  case 3:
    ;
    int btmp = 0;

    printf ("=============================\n        Estoque Atual\n=============================\n");

    for (size_t i = 0; i < prod_amount; i ++)
      if (prod[i].amount != 0)
      {
        printf ("Nome: %s\nQuantidade: %lu\n\n", prod[i].name, prod[i].amount);
        btmp = 1;
      }

    if (!btmp)
      printf ("O estoque está vazio.\n");

    break;

  case 4:
      return 0;
  }

  return menu_continue ();
}

signed menu (void)
{
  uint8_t op;

  printf ("================================\n   Controle de Estoque\n================================\nSelecione uma opção:\n1. Adicionar Item\n2. Remover Item\n3. Listar Estoque\n4. Sair\nOpção: ");

  if (scanf ("%hhu", &op) != 1)
  { NUMBER_ERR; return menu (); }

  return do_op (op);
}

signed main (void)
{
  read_file ();
  signed r = menu ();
  if (r == 0) printf ("Obrigado por usar o Controle de Estoques! Até a próxima.\n");
  update_file ();
  return r;
}
