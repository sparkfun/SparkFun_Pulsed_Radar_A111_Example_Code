BUILD_LIBS += $(OUT_LIB_DIR)/libcustomer.a

$(OUT_LIB_DIR)/libcustomer.a : $(addprefix $(OUT_OBJ_DIR)/,$(notdir $(patsubst %.c,%.o,$(wildcard source/acc_driver_*.c)))) \
		    $(addprefix $(OUT_OBJ_DIR)/,$(notdir $(patsubst %.c,%.o,$(wildcard source/acc_device_*.c)))) \
		    $(addprefix $(OUT_OBJ_DIR)/,$(notdir $(patsubst %.c,%.o,$(wildcard source/acc_log_*.c)))) \
		    $(addprefix $(OUT_OBJ_DIR)/,$(notdir $(patsubst %.c,%.o,$(wildcard source/acc_hal_integration_*.c))))
	@echo "    Creating archive $(notdir $@)"
	$(SUPPRESS)rm -f $@
	$(SUPPRESS)$(AR) cr $@ $^
