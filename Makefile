LIBS  = -ljson
CFLAGS = -Wall

objects = json_test1 json_test2 json_server json_client
all: $(objects)

$(objects): %: %.c
	$(CXX) $(CFLAGS) -o $@ $<  $(LIBS) -lpthread

clean: 
	rm -rf $(objects)
	
