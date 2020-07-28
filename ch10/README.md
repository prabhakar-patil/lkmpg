struct tty_struct defination not available in recent linux kernel headers.
It turns out to compilations error.
/home/e0410439/temp/repo/lkmpg/ch10/print_string.c:17:23: error: dereferencing pointer to incomplete type ‘struct signal_struct’
  tty = current->signal->tty;

