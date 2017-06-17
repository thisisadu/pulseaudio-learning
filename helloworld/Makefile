include $(TOPDIR)/rules.mk
  
PKG_NAME:=helloworld
PKG_RELEASE:=1
  
  
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
  
include $(INCLUDE_DIR)/package.mk  
  
   
define Package/helloworld  
	SECTION:=utils  
	CATEGORY:=Utilities  
	TITLE:=Helloworld -- prints a snarky message  
	DEPENDS:=+pulseaudio
endef  
  
  
define Package/helloworld/description  
	If you can't figure out what this program does, you're probably  
	brain-dead and need immediate medical attention.  
endef  
  
define Build/Prepare  
	mkdir -p $(PKG_BUILD_DIR)  
	$(CP) ./src/* $(PKG_BUILD_DIR)/  
endef  
  
  
define Package/helloworld/install  
	$(INSTALL_DIR) $(1)/bin  
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/helloworld $(1)/bin/  
endef  
  
$(eval $(call BuildPackage,helloworld))
