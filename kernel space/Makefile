obj-m := sample.o  
KERNELBUILD := /lib/modules/`uname -r`/build  
default:  
	@echo "  BUILD kmod"  
	@make -C $(KERNELBUILD) M=$(shell pwd) modules  

clean:  
	@echo "  CLEAN kmod"  
	@rm -rf *.o  
	@rm -rf .depend .*.cmd *.ko *.mod.c .tmp_versions *.symvers .*.d  
