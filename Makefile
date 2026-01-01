.PHONY: xx
xx:
	if [ -d "build" ]; then \
		cd build && cmake .. && $(MAKE); \
	else \
		mkdir build; \
		cd build && cmake ..; \
	fi
%:
	if [ -d "build" ]; then \
		cd build && cmake .. && +$(MAKE) $@; \
	else \
		mkdir build; \
		cd build && cmake ..; \
	fi
clean:
	if [ -d "build" ]; then \
		cd build && $(MAKE) clean && cd ..; \
		rm -rf build; \
	fi