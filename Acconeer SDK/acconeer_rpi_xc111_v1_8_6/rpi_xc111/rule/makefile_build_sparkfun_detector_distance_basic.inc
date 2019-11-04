BUILD_ALL += $(OUT_DIR)/sparkfun_detector_distance_basic

$(OUT_DIR)/sparkfun_detector_distance_basic : \
					$(OUT_OBJ_DIR)/example_detector_distance_basic.o \
					$(OUT_OBJ_DIR)/acc_detector_distance_basic.o \
					libacconeer.a \
					libacconeer_sensor.a \
					libcustomer.a \
					libacc_service.a \
					$(OUT_OBJ_DIR)/acc_board_rpi_sparkfun.o
	@echo "    Linking $(notdir $@)"
	@mkdir -p $(OUT_DIR)
	@$(LINK.o) -Wl,--start-group $^ -Wl,--end-group $(LOADLIBES) $(LDLIBS) -o $@
