HEADERS = echo_c_functions.h echo_s_functions.h

output:echo_c.o echo_c_functions.o echo_s.o echo_s_functions.o log_s.o
        cc -o output echo_c.o echo_c_functions.o echo_s.o echo_s_functions.o log_s.o
echo_c.o: echo_c.c $(HEADERS)
  cc -c echo_c.c
echo_c_functions.o: echo_c_functions.c $(HEADERS)
  cc -c echo_c_functions.c 
 echo_s.o: echo_s.c $(HEADERS)
  cc -c echo_s.c
echo_s_functions.o: echo_s_functions.c $(HEADERS)
  cc -c echo_s_functions.c
log_s.o: log_s.c $(HEADERS)
  cc -c log_s.c
  
  clean: 
  rm *.o output
 
