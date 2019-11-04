BUILD_ALL += $(OUT_DIR)/example_detector_presence_rpi_sparkx_a111_r2c

$(OUT_DIR)/example_detector_presence_rpi_sparkx_a111_r2c : \
					$(OUT_OBJ_DIR)/example_detector_presence.o \
					libacconeer.a \
					libacconeer_sensor.a \
					libcustomer.a \
					libacc_service.a \
					libacc_detector_presence.a \
					$(OUT_OBJ_DIR)/acc_board_rpi_sparkx.o
	@echo "    Linking $(notdir $@)"
	$(SUPPRESS)mkdir -p $(OUT_DIR)
	$(SUPPRESS)$(LINK.o) -Wl,--start-group $^ -Wl,--end-group $(LOADLIBES) $(LDLIBS) -o $@


BUILD_ALL += $(OUT_DIR)/ref_app_smart_presence_rpi_sparkx_a111_r2c

$(OUT_DIR)/ref_app_smart_presence_rpi_sparkx_a111_r2c : \
					$(OUT_OBJ_DIR)/ref_app_smart_presence.o \
					libacconeer.a \
					libacconeer_sensor.a \
					libcustomer.a \
					libacc_service.a \
					libacc_detector_presence.a \
					$(OUT_OBJ_DIR)/acc_board_rpi_sparkx.o
	@echo "    Linking $(notdir $@)"
	$(SUPPRESS)mkdir -p $(OUT_DIR)
	$(SUPPRESS)$(LINK.o) -Wl,--start-group $^ -Wl,--end-group $(LOADLIBES) $(LDLIBS) -o $@
