BUILD_ALL += $(OUT_DIR)/sparkfun_detector_distance

$(OUT_DIR)/sparkfun_detector_distance : \
					$(OUT_OBJ_DIR)/sparkfun_detector_distance.o \
					libacconeer.a \  // yes
					libacc_service.a \
          libcustomer.a \
					libacc_detector_distance_peak.a \
          $(OUT_OBJ_DIR)/acc_board_rpi_sparkfun.o
	@echo "    Linking $(notdir $@)"
	@mkdir -p $(OUT_DIR)
	@$(LINK.o) -Wl,--start-group $^ -Wl,--end-group $(LOADLIBES) $(LDLIBS) -o $@
  
      // These SOURCE files no longer exist
					//libacconeer_a111_r2c.a \
					//libacc_local_server.a \
					//libacc_message_driver_dummy.a \
          //out/libcustomer.a \
					//libacc_envelope.a \
					//libacc_power_bins.a \

