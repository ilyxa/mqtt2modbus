#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <modbus.h>
#include <mosquitto.h>

#define DEVICE "/dev/ttyS0"
#define MQTT_PREFIX "plc/"
#define MQTT_HOST "localhost"
#define MQTT_PORT 1883
#define MQTT_KEEPALIVE 60

typedef struct
{
    const char *identifier;
    int address;
    const char *label;

} output_t;

const output_t outputs[] =
{
    { .identifier = "y402", .address = 0x2081, .label = "SV 1NP SCHODY" },
    { .identifier = "y401", .address = 0x2080, .label = "SV 2NP KUCHYN LINKA" },
    { .identifier = "y403", .address = 0x2082, .label = "SV 1NP OBYVAK KRB STR" },
    { .identifier = "y404", .address = 0x2083, .label = "SV 1NP OBYVAK KRB KRJ" },
    { .identifier = "y405", .address = 0x2084, .label = "SV 2NP LOZNICE KRJ" },
    { .identifier = "y406", .address = 0x2085, .label = "SV 2NP LOZNICE STR" },
    { .identifier = "y407", .address = 0x2086, .label = "SV 1NP WC" },
    { .identifier = "y409", .address = 0x2088, .label = "SV 0NP KOMORA KRJ" },
    { .identifier = "y410", .address = 0x2089, .label = "SV 0NP KOMORA STR" },
    { .identifier = "y411", .address = 0x208a, .label = "SV 0NP GARAZ 1" },
    { .identifier = "y412", .address = 0x208b, .label = "SV 0NP VRATA" },
    { .identifier = "y413", .address = 0x208c, .label = "SV 0NP GARAZ 2" },
    { .identifier = "y414", .address = 0x208d, .label = "SV 0NP GARAZ 3" },
    { .identifier = "y415", .address = 0x208e, .label = "SV 0NP SCHODY" },
    { .identifier = "y416", .address = 0x208f, .label = "SV 0NP CHODBA" },
    { .identifier = "y501", .address = 0x20a0, .label = "SV 1NP VCHOD" },
    { .identifier = "y503", .address = 0x20a2, .label = "SV 1NP PRISTAVBA" },
    { .identifier = "y505", .address = 0x20a4, .label = "SV 0NP SPIZIRNA STR" },
    { .identifier = "y506", .address = 0x20a5, .label = "SV 0NP SPIZIRNA KRJ" },
    { .identifier = "y509", .address = 0x20a8, .label = "SV 2NP LOZNICE BOD L" },
    { .identifier = "y510", .address = 0x20a9, .label = "SV 2NP LOZNICE BOD P" },
    { .identifier = "y511", .address = 0x20aa, .label = "SV 1NP KOUPELNA ZRCADLO" },
    { .identifier = "y512", .address = 0x20ab, .label = "ZA 1NP KOUPELNA ZRCADLO" },
    { .identifier = "y513", .address = 0x20ac, .label = "SV 2NP KOUPELNA" },
    { .identifier = "y514", .address = 0x20ad, .label = "SV 2NP KOUPELNA SPRCHA" },
    { .identifier = "y515", .address = 0x20ae, .label = "SV 2NP JIDELNA KRJ" },
    { .identifier = "y516", .address = 0x20af, .label = "SV 2NP JIDELNA STR" },
    { .identifier = "y603", .address = 0x20c2, .label = "SV 2NP CHODBA" },
    { .identifier = "y605", .address = 0x20c4, .label = "SV 2NP KUCHYN STRED" },
    { .identifier = "y606", .address = 0x20c5, .label = "SV 2NP KUCHYN BAR" },
    { .identifier = "y607", .address = 0x20c6, .label = "SV 2NP OBYVAK TV KRJ" },
    { .identifier = "y608", .address = 0x20c7, .label = "SV 2NP OBYVAK TV STR" },
    { .identifier = "y609", .address = 0x20c8, .label = "SV 1NP JIDELNA KRJ" },
    { .identifier = "y610", .address = 0x20c9, .label = "SV 1NP JIDELNA STR" },
    { .identifier = "y611", .address = 0x20ca, .label = "SV 1NP KOUPELNA BOD" },
    { .identifier = "y613", .address = 0x20cc, .label = "SV 1NP KUCHYN LINKA" },
    { .identifier = "y614", .address = 0x20cd, .label = "SV 1NP KUCHYN TROUBA" },
    { .identifier = "y615", .address = 0x20ce, .label = "SV 1NP KUCHYN ZARIVKA L" },
    { .identifier = "y616", .address = 0x20cf, .label = "SV 1NP KUCHYN ZARIVKA P" },
    { .identifier = "y701", .address = 0x20e0, .label = "SV 2NP OBYVAK STRED KRJ" },
    { .identifier = "y702", .address = 0x20e1, .label = "SV 2NP OBYVAK STRED STR" },
    { .identifier = "y703", .address = 0x20e2, .label = "SV 1NP OBYVAK OKNO STR" },
    { .identifier = "y704", .address = 0x20e3, .label = "SV 1NP OBYVAK OKNO KRJ" },
    { .identifier = "y705", .address = 0x20e4, .label = "SV 1NP CHODBA PRISTAVBA" },
    { .identifier = "y706", .address = 0x20e5, .label = "SV 1NP CHODBA KOUPELNA" },
    { .identifier = "y707", .address = 0x20e6, .label = "SV 2NP KOUPELNA ZRCADLO" },
    { .identifier = "y709", .address = 0x20e8, .label = "SV 2NP PRACOVNA KRJ" },
    { .identifier = "y710", .address = 0x20e9, .label = "SV 2NP PRACOVNA STR" },
    { .identifier = "y711", .address = 0x20ea, .label = "SV 2NP KUCHYN SOKL" },
    { .identifier = "y712", .address = 0x20eb, .label = "SV 2NP KUCHYN SIKMINA" },
    { .identifier = "y713", .address = 0x20ec, .label = "SV 1NP LOZNICE STR" },
    { .identifier = "y714", .address = 0x20ed, .label = "SV 1NP LOZNICE KRJ" },
    { .identifier = "y715", .address = 0x20ee, .label = "SV 1NP TECH MISTNOST" },
    { .identifier = "y801", .address = 0x2100, .label = "SV 3NP POKOJ 1 JIH" },
    { .identifier = "y802", .address = 0x2101, .label = "SV 3NP POKOJ 1 SEVER" },
    { .identifier = "y803", .address = 0x2102, .label = "SV 3NP CHODBA ZAPAD" },
    { .identifier = "y804", .address = 0x2103, .label = "SV 2NP SCHODY" },
    { .identifier = "y805", .address = 0x2104, .label = "SV 3NP CHODBA VYCHOD" },
    { .identifier = "y807", .address = 0x2106, .label = "SV 3NP POKOJ 2 JIH" },
    { .identifier = "y808", .address = 0x2107, .label = "SV 3NP POKOJ 2 SEVER" },

    { .identifier = "c101", .address = 0x4064, .label = "SV DUM SV" },
    { .identifier = "c102", .address = 0x4065, .label = "SV DUM JV" },
    { .identifier = "c103", .address = 0x4066, .label = "SV DUM JZ" },
    { .identifier = "c104", .address = 0x4067, .label = "SV DUM SZ" },
    { .identifier = "c105", .address = 0x4068, .label = "SV PARKING" },
    { .identifier = "c106", .address = 0x4069, .label = "SV ZAHR J" },
    { .identifier = "c107", .address = 0x406a, .label = "SV RODODEN" },
    { .identifier = "c108", .address = 0x406b, .label = "SV GARAZ" },
    { .identifier = "c109", .address = 0x406c, .label = "SV CHALOUP" },
    { .identifier = "c110", .address = 0x406d, .label = "SV STUDNA" },
    { .identifier = "c111", .address = 0x406e, .label = "SV ZAHR S" },

    { .identifier = "c200", .address = 0x40c7, .label = "MB LATCH" },
    { .identifier = "c201", .address = 0x40c8, .label = "MB SENDING" },
    { .identifier = "c202", .address = 0x40c9, .label = "MB SUCCESS" },
    { .identifier = "c203", .address = 0x40ca, .label = "MB ERROR" },

    { .identifier = NULL, .address = 0, .label = NULL }
};

modbus_t *mb = NULL;

struct mosquitto *mosq = NULL;

void mb_connect(const char *device)
{
    mb = modbus_new_rtu(device, 38400, 'O', 8, 1);

    if (mb == NULL)
    {
        fprintf(stderr, "modbus_new_rtu: Call failed\n");
        exit(EXIT_FAILURE);
    }

    if (modbus_set_slave(mb, 1) == -1)
    {
        fprintf(stderr, "modbus_set_slave: %s\n", modbus_strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (modbus_connect(mb) == -1)
    {
        fprintf(stderr, "modbus_connect: %s\n", modbus_strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void mb_write_bit(int address, bool state)
{
    if (modbus_write_bit(mb, address, state) == -1)
    {
        fprintf(stderr, "modbus_write_bit: %s\n", modbus_strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void mb_read_bit(int address, bool *state)
{
    uint8_t buf;

    if (modbus_read_bits(mb, address, 1, &buf) == -1)
    {
        fprintf(stderr, "modbus_read_bits: %s\n", modbus_strerror(errno));
        exit(EXIT_FAILURE);
    }

    *state = buf;
}

void mqtt_connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
    if (rc == 0)
    {
        for (size_t i = 0; outputs[i].identifier != NULL; i++)
        {
            char *topic = malloc(strlen(MQTT_PREFIX) + strlen(outputs[i].identifier) + 4 + 1);

            if (topic == NULL)
            {
                fprintf(stderr, "malloc: Not enough memory\n");
                exit(EXIT_FAILURE);
            }

            strcpy(topic, MQTT_PREFIX);
            strcat(topic, outputs[i].identifier);
            strcat(topic, "/set");

            if (mosquitto_subscribe(mosq, NULL, topic, 1) != MOSQ_ERR_SUCCESS)
            {
                fprintf(stderr, "mosquitto_subscribe: Call failed\n");
            }

            free(topic);
        }

        for (size_t i = 0; outputs[i].identifier != NULL; i++)
        {
            char *topic = malloc(strlen(MQTT_PREFIX) + strlen(outputs[i].identifier) + 7 + 1);

            if (topic == NULL)
            {
                fprintf(stderr, "malloc: Not enough memory\n");
                exit(EXIT_FAILURE);
            }

            strcpy(topic, MQTT_PREFIX);
            strcat(topic, outputs[i].identifier);
            strcat(topic, "/toggle");

            if (mosquitto_subscribe(mosq, NULL, topic, 1) != MOSQ_ERR_SUCCESS)
            {
                fprintf(stderr, "mosquitto_subscribe: Call failed\n");
            }

            free(topic);
        }
    }
    else
    {
        fprintf(stderr, "mqtt_connect_callback: Connection failed\n");
    }
}

void mqtt_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    for (size_t i = 0; outputs[i].identifier != NULL; i++)
    {
        char *topic = malloc(strlen(MQTT_PREFIX) + strlen(outputs[i].identifier) + 4 + 1);

        if (topic == NULL)
        {
            fprintf(stderr, "malloc: Not enough memory\n");
            exit(EXIT_FAILURE);
        }

        strcpy(topic, MQTT_PREFIX);
        strcat(topic, outputs[i].identifier);
        strcat(topic, "/set");

        if (strcmp(message->topic, topic) == 0)
        {
            bool ok = false;

            if (message->payloadlen == 4 && strcmp(message->payload, "true") == 0)
            {
                mb_write_bit(outputs[i].address, true);

                ok = true;
            }
            else if (message->payloadlen == 5 && strcmp(message->payload, "false") == 0)
            {
                mb_write_bit(outputs[i].address, false);

                ok = true;
            }

            if (ok)
            {
                char *response = malloc(strlen(topic) + 3 + 1);

                if (response == NULL)
                {
                    fprintf(stderr, "malloc: Not enough memory\n");
                    exit(EXIT_FAILURE);
                }

                strcpy(response, topic);
                strcat(response, "/ok");

                if (mosquitto_publish(mosq, NULL, response, 0, NULL, 1, false) != MOSQ_ERR_SUCCESS)
                {
                    fprintf(stderr, "mosquitto_publish: Call failed\n");
                }

                free(response);
            }
        }

        free(topic);
    }

    for (size_t i = 0; outputs[i].identifier != NULL; i++)
    {
        char *topic = malloc(strlen(MQTT_PREFIX) + strlen(outputs[i].identifier) + 7 + 1);

        if (topic == NULL)
        {
            fprintf(stderr, "malloc: Not enough memory\n");
            exit(EXIT_FAILURE);
        }

        strcpy(topic, MQTT_PREFIX);
        strcat(topic, outputs[i].identifier);
        strcat(topic, "/toggle");

        if (strcmp(message->topic, topic) == 0)
        {
            bool ok = false;

            if (message->payloadlen == 0)
            {
                bool state;

                mb_read_bit(outputs[i].address, &state);
                mb_write_bit(outputs[i].address, !state);

                ok = true;
            }

            if (ok)
            {
                char *response = malloc(strlen(topic) + 3 + 1);

                if (response == NULL)
                {
                    fprintf(stderr, "malloc: Not enough memory\n");
                    exit(EXIT_FAILURE);
                }

                strcpy(response, topic);
                strcat(response, "/ok");

                if (mosquitto_publish(mosq, NULL, response, 0, NULL, 1, false) != MOSQ_ERR_SUCCESS)
                {
                    fprintf(stderr, "mosquitto_publish: Call failed\n");
                }

                free(response);
            }
        }

        free(topic);
    }
}

void cleanup(void)
{
    if (mosq != NULL)
    {
        mosquitto_destroy(mosq);
    }

    mosquitto_lib_cleanup();

    if (mb != NULL)
    {
        modbus_close(mb);
        modbus_free(mb);
    }
}

int main(int argc, char **argv)
{
    if (atexit(cleanup) != 0)
    {
        fprintf(stderr, "atexit: Call failed\n");
        exit(EXIT_FAILURE);
    }

    mb_connect(DEVICE);

    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);

    if (mosq == NULL)
    {
        fprintf(stderr, "mosquitto_new: Call failed\n");
        exit(EXIT_FAILURE);
    }

    mosquitto_connect_callback_set(mosq, mqtt_connect_callback);
    mosquitto_message_callback_set(mosq, mqtt_message_callback);

    if (mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, MQTT_KEEPALIVE) != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "mosquitto_connect: Unable to connect\n");
        exit(EXIT_FAILURE);
    }

    mosquitto_loop_forever(mosq, -1, 1);

    exit(EXIT_SUCCESS);
}
