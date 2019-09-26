BUILD_ALL += utils/acc_streaming_server_rpi_xc111_r4a_xr111-3_r1c_a111_r2c

utils/acc_streaming_server_rpi_xc111_r4a_xr111-3_r1c_a111_r2c : \
					acc_streaming_server.o \
					cJSON.o \
					libacconeer.a \
					libacconeer_sensor.a \
					libcustomer.a \
					libacc_service.a \
					$(OUT_OBJ_DIR)/acc_board_rpi_xc111_r4a_xr111-3_r1c.o
	@echo "    Linking $(notdir $@)"
	$(SUPPRESS)mkdir -p utils
	$(SUPPRESS)$(LINK.o) -Wl,--start-group $^ -Wl,--end-group $(LOADLIBES) $(LDLIBS) -o $@
