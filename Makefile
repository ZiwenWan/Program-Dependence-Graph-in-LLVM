# Ensure there is a build folder
ODIR = ./build
output_folder := $(shell mkdir -p $(ODIR))

all: libtmplugin.so

libtmplugin.so:
	@echo Configuring...
	@cd $(ODIR) && cmake ..
	@echo Building...
	@$(MAKE) -C $(ODIR)

clean:
	@echo Cleaning up...
	@rm -rf $(ODIR)