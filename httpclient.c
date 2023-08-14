#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/http_client.h"
char myBuff[4096];

void result(void *arg, httpc_result_t httpc_result,
            u32_t rx_content_len, u32_t srv_res, err_t err)

{
    printf("\ntransfer complete\n");
    printf("local result=%d\n", httpc_result);
    printf("http result=%d\n", srv_res);
}

err_t headers(httpc_state_t *connection, void *arg,
              struct pbuf *hdr, u16_t hdr_len, u32_t content_len)
{
    printf("\nheaders recieved\n");
    printf("content length=%d\n", content_len);
    printf("header length %d\n", hdr_len);
    pbuf_copy_partial(hdr, myBuff, hdr->tot_len, 0);
    printf("headers \n");
    printf("%s", myBuff);
    return ERR_OK;
}

err_t body(void *arg, struct altcp_pcb *conn,
           struct pbuf *p, err_t err)
{
    printf("body\n");
    pbuf_copy_partial(p, myBuff, p->tot_len, 0);
    int s = strlen(myBuff);
    printf("body length %d\n", s);
    printf("%s", myBuff);
    return ERR_OK;
}

int setup(uint32_t country, const char *ssid,
          const char *pass, uint32_t auth,
          const char *hostname, ip_addr_t *ip,
          ip_addr_t *mask, ip_addr_t *gw)
{
    if (cyw43_arch_init_with_country(country))
    {
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    if (hostname != NULL)
    {
        netif_set_hostname(netif_default, hostname);
    }

    // if (cyw43_arch_wifi_connect_async(ssid, pass, auth))
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, auth, 10000))
    {
        return 2;
    }

    int flashrate = 1000;
    int status = CYW43_LINK_UP + 1;
    while (status >= 0 && status != CYW43_LINK_UP)
    {
        printf("Retrying connection\n");
        int new_status = cyw43_tcpip_link_status(
            &cyw43_state, CYW43_ITF_STA);
        if (new_status != status)
        {
            status = new_status;
            flashrate = flashrate / (status + 1);
            printf("connect status: %d %d\n",
                   status, flashrate);
        }
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(flashrate);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(flashrate);
    }
    if (status < 0)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    }
    else
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        if (ip != NULL)
        {
            netif_set_ipaddr(netif_default, ip);
        }
        if (mask != NULL)
        {
            netif_set_netmask(netif_default, mask);
        }
        if (gw != NULL)
        {
            netif_set_gw(netif_default, gw);
        }
        printf("IP: %s\n",
               ip4addr_ntoa(netif_ip_addr4(netif_default)));
        printf("Mask: %s\n",
               ip4addr_ntoa(netif_ip_netmask4(netif_default)));
        printf("Gateway: %s\n",
               ip4addr_ntoa(netif_ip_gw4(netif_default)));
        printf("Host Name: %s\n",
               netif_get_hostname(netif_default));
    }
    return status;
}
#ifndef WIFI_SSID
#define WIFI_SSID "WIFI_SSID"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "WIFI_PASS"
#endif
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;
uint32_t country = CYW43_COUNTRY_NETHERLANDS;
uint32_t auth = CYW43_AUTH_WPA2_MIXED_PSK;
int main()
{
    stdio_init_all();
    for (int i = 0; i < 3; i++)
    {
        printf("Wake-up USB\n");
        sleep_ms(1000);
    }
    printf("SSID: %s\n", ssid);
    printf("PASS: %s\n", pass);

    printf("Country: %s\n", WIFI_COUNTRY);
    printf("Remote domain: %s\n", REMOTE_DOMAIN);
    printf("Relative url: %s\n", RELATIVE_URL);
    if (strlen(WIFI_COUNTRY) == 2)
    {
        country = CYW43_COUNTRY(WIFI_COUNTRY[0],
                                WIFI_COUNTRY[1], 0);
    }
    else
    {
        printf("Invalid country code, halting\n");
        while (1)
        {
            tight_loop_contents();
        }
    }

    if (setup(country, ssid, pass, auth,
              "MyPicoW", NULL, NULL, NULL) == CYW43_LINK_UP)
    {
        uint16_t port = 80;
        httpc_connection_t settings;

        settings.result_fn = result;
        settings.headers_done_fn = headers;

        err_t err = httpc_get_file_dns(
            REMOTE_DOMAIN,
            80,
            RELATIVE_URL,
            &settings,
            body,
            NULL,
            NULL);

        printf("status %d \n", err);
    }
    while (true)
    {
        sleep_ms(500);
    }
}