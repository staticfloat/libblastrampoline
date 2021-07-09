include ../../src/Make.inc

all: $(prefix)/inconsolable_test$(EXE)

$(prefix):
	@mkdir -p $@

$(prefix)/inconsolable_test$(EXE): inconsolable_test.c | $(prefix)
	@$(CC) -o $@ $(LBT_CFLAGS) $^ $(LBT_LDFLAGS)

clean:
	@rm -f $(prefix)/inconsolable_test$(EXE)

run: $(prefix)/inconsolable_test$(EXE)
	@$(prefix)/inconsolable_test$(EXE)
