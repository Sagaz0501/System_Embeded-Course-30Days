CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c11

# ejecutables
SWITCH=boton_switch
TOGGLE=boton_toggle

all: $(SWITCH) $(TOGGLE)

$(SWITCH): main.c
	$(CC) $(CFLAGS) main.c -o $(SWITCH)

$(TOGGLE): main_toggle.c
	$(CC) $(CFLAGS) main_toggle.c -o $(TOGGLE)

run-switch: $(SWITCH)
	./$(SWITCH)

run-toggle: $(TOGGLE)
	./$(TOGGLE)

clean:
	rm -f $(SWITCH) $(TOGGLE)

.PHONY: all clean run-switch run-toggle
