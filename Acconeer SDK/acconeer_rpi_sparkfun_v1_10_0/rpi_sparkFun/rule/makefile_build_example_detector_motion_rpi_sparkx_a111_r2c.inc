BUILD_ALL += $(OUT_DIR)/example_detector_motion_rpi_sparkx_a111_r2c

$(OUT_DIR)/example_detector_motion_rpi_sparkx_a111_r2c : \
					$(OUT_OBJ_DIR)/example_detector_motion.o \
					libacconeer.a \
					libacconeer_sensor.a \
					libcustomer.a \
					libacc_detector_motion.a \
					libacc_service.a \
					$(OUT_OBJ_DIR)/acc_board_rpi_sparkx.o
	@echo "    Linking $(notdir $@)"
	$(SUPPRESS)mkdir -p $(OUT_DIR)
	$(SUPPRESS)$(LINK.o) -Wl,--start-group $^ -Wl,--end-group $(LOADLIBES) $(LDLIBS) -o $@
