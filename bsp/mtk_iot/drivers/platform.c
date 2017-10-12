#include <rthw.h>
#include <rtthread.h>
#include "platform.h"
//
#include "string.h"

#include "wifi_api.h"
#include "wifi_lwip_helper.h"

#include "ethernetif.h"

#define	MAC_ADDR_LEN	6
struct rt_wifi_dev
{
	/* inherit from wifi device */
	struct eth_device parent;
	
	/* interface address info */
	rt_uint8_t dev_addr[MAC_ADDR_LEN];
};
static struct rt_wifi_dev mtk_wifi_sta_device;
static struct rt_wifi_dev mtk_wifi_ap_device;



rt_err_t rt_mtk_wifi_init(rt_device_t dev)
{
    /* User initial the parameters for wifi initial process,  system will determin which wifi operation mode
     * will be started , and adopt which settings for the specific mode while wifi initial process is running*/
    wifi_config_t config = {0};
    config.opmode = WIFI_MODE_STA_ONLY;
    strcpy((char *)config.sta_config.ssid, (const char *)"360WiFI-FB321E");
    strcpy((char *)config.sta_config.password, (const char *)"20112012pw707");
    //    strcpy((char *)config.sta_config.ssid, (const char *)"TP-LINK_487880");
    //    strcpy((char *)config.sta_config.password, (const char *)"chenyisong12345");
    config.sta_config.ssid_length = strlen((const char *)config.sta_config.ssid);
    config.sta_config.password_length = strlen((const char *)config.sta_config.password);

    /* Initialize wifi stack and register wifi init complete event handler,
     * notes:  the wifi initial process will be implemented and finished while system task scheduler is running.*/
    wifi_init(&config, NULL);

    /* Tcpip stack and net interface initialization,  dhcp client, dhcp server process initialization*/
    lwip_network_init(config.opmode);
    lwip_net_start(config.opmode);

    return RT_EOK;
}

rt_err_t rt_mtk_wifi_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

rt_err_t rt_mtk_wifi_close(rt_device_t dev)
{
    return RT_EOK;
}
rt_size_t rt_mtk_wifi_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_set_errno(RT_ENOSYS);
    return RT_EOK;
}

rt_size_t rt_mtk_wifi_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    rt_set_errno(RT_ENOSYS);
    return RT_EOK;
}

rt_err_t rt_mtk_wifi_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    switch(cmd)
    {
        case NIOCTL_GADDR:
            if(args)
            {
                if(rt_strcmp(dev->parent.name, "st") == 0)
                    rt_memcpy(args, mtk_wifi_sta_device.dev_addr, MAC_ADDR_LEN);
                else if(rt_strcmp(dev->parent.name, "ap") == 0)
                    rt_memcpy(args, mtk_wifi_ap_device.dev_addr, MAC_ADDR_LEN);
            }
        break;

        default:
        break;
    }
    
   return RT_EOK;
}

struct pbuf* rt_mtk_wifi_rx(rt_device_t dev)
{
    
}

rt_err_t rt_mtk_wifi_tx(rt_device_t dev, struct pbuf* p)
{
    return RT_EOK;
}

void rt_hw_wifi_init(void)
{
    //       interface in the link-list: STA -> AP -> NULL.
    if (0 > wifi_config_get_mac_address(WIFI_PORT_STA, (uint8_t *)mtk_wifi_sta_device.dev_addr) ||
        0 > wifi_config_get_mac_address(WIFI_PORT_AP, (uint8_t *)mtk_wifi_ap_device.dev_addr)) {
        rt_kprintf("get mac fail!\n");
        return;
    }
    rt_kprintf("sta_mac: %02x %02x %02x %02x %02x %02x", \
                mtk_wifi_sta_device.dev_addr[0], mtk_wifi_sta_device.dev_addr[1], mtk_wifi_sta_device.dev_addr[2], \
                mtk_wifi_sta_device.dev_addr[3], mtk_wifi_sta_device.dev_addr[4], mtk_wifi_sta_device.dev_addr[5]);

    mtk_wifi_sta_device.parent.parent.init = rt_mtk_wifi_init;
    mtk_wifi_sta_device.parent.parent.open = rt_mtk_wifi_open;
    mtk_wifi_sta_device.parent.parent.close = rt_mtk_wifi_close;
    mtk_wifi_sta_device.parent.parent.read = rt_mtk_wifi_read;
    mtk_wifi_sta_device.parent.parent.write = rt_mtk_wifi_write;
    mtk_wifi_sta_device.parent.parent.control = rt_mtk_wifi_control;
    mtk_wifi_sta_device.parent.parent.user_data = RT_NULL;
    //
    mtk_wifi_sta_device.parent.eth_tx = rt_mtk_wifi_tx;
    mtk_wifi_sta_device.parent.eth_rx = rt_mtk_wifi_rx;

    eth_device_init(&mtk_wifi_sta_device.parent, "st");

    rt_kprintf("ap_mac: %02x %02x %02x %02x %02x %02x", \
                mtk_wifi_ap_device.dev_addr[0], mtk_wifi_ap_device.dev_addr[1], mtk_wifi_ap_device.dev_addr[2], \
                mtk_wifi_ap_device.dev_addr[3], mtk_wifi_ap_device.dev_addr[4], mtk_wifi_ap_device.dev_addr[5]);
    
    mtk_wifi_ap_device.parent.parent.init = rt_mtk_wifi_init;
    mtk_wifi_ap_device.parent.parent.open = rt_mtk_wifi_open;
    mtk_wifi_ap_device.parent.parent.close = rt_mtk_wifi_close;
    mtk_wifi_ap_device.parent.parent.read = rt_mtk_wifi_read;
    mtk_wifi_ap_device.parent.parent.write = rt_mtk_wifi_write;
    mtk_wifi_ap_device.parent.parent.control = rt_mtk_wifi_control;
    mtk_wifi_ap_device.parent.parent.user_data = RT_NULL;
    //
    mtk_wifi_ap_device.parent.eth_tx = rt_mtk_wifi_tx;
    mtk_wifi_ap_device.parent.eth_rx = rt_mtk_wifi_rx;
    
    eth_device_init(&mtk_wifi_ap_device.parent, "ap");
}

void rt_platform_init(void)
{
    rt_hw_wifi_init();
}
