BUILD_ALL += utils/acc_service_data_logger_rpi_xc111_r4a_xr111-3_r1c_a111_r2c

utils/acc_service_data_logger_rpi_xc111_r4a_xr111-3_r1c_a111_r2c : \
					$(OUT_OBJ_DIR)/acc_service_data_logger.o \
					libacconeer.a \
					libacconeer_sensor.a \
					libcustomer.a \
					libacc_service.a \
					$(OUT_OBJ_DIR)/acc_board_rpi_xc111_r4a_xr111-3_r1c.o
	@echo "    Linking $(notdir $@)"
	$(SUPPRESS)mkdir -p utils
	$(SUPPRESS)$(LINK.o) -Wl,--start-group $^ -Wl,--end-group $(LOADLIBES) $(LDLIBS) -o $@
