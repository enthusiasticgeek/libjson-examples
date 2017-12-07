LIBS  = -ljson
CFLAGS = -Wall

objects = json_test1 json_test2
all: $(objects)

$(objects): %: %.c
	$(CXX) $(CFLAGS) -o $@ $<  $(LIBS)

clean: 
	rm -rf $(objects)
	
