/*
 * Zephyr RTOS Data Logger
 * 
 * Demonstrates multi-threaded sensor data logging with:
 * - Periodic sensor sampling
 * - Message queue communication
 * - Priority-based scheduling
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(datalogger, LOG_LEVEL_INF);

/* Thread stack sizes and priorities */
#define SENSOR_STACK_SIZE 1024
#define LOGGER_STACK_SIZE 1024
#define SENSOR_PRIORITY 7
#define LOGGER_PRIORITY 8

/* Message queue configuration */
#define MSG_QUEUE_SIZE 10

/* Sampling interval in milliseconds */
#define SAMPLE_INTERVAL_MS 1000

/* Sensor data structure */
struct sensor_data {
    uint32_t timestamp;
    int32_t temperature;    /* simulated: milli-degrees C */
    int32_t humidity;       /* simulated: milli-percent */
    uint32_t sequence;
};

/* Define message queue */
K_MSGQ_DEFINE(sensor_msgq, sizeof(struct sensor_data), MSG_QUEUE_SIZE, 4);

/* Thread stacks */
K_THREAD_STACK_DEFINE(sensor_stack, SENSOR_STACK_SIZE);
K_THREAD_STACK_DEFINE(logger_stack, LOGGER_STACK_SIZE);

/* Thread data structures */
static struct k_thread sensor_thread_data;
static struct k_thread logger_thread_data;

/* Simulated sensor reading */
static void read_sensors(struct sensor_data *data, uint32_t seq)
{
    data->timestamp = k_uptime_get_32();
    /* Simulate temperature: 20-30 degrees C with some variation */
    data->temperature = 25000 + (k_uptime_get_32() % 5000) - 2500;
    /* Simulate humidity: 40-60% */
    data->humidity = 50000 + (k_uptime_get_32() % 10000) - 5000;
    data->sequence = seq;
}

/* Sensor sampling thread */
static void sensor_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    struct sensor_data data;
    uint32_t sequence = 0;

    LOG_INF("Sensor thread started");

    while (1) {
        read_sensors(&data, sequence++);

        /* Send to message queue, non-blocking */
        int ret = k_msgq_put(&sensor_msgq, &data, K_NO_WAIT);
        if (ret != 0) {
            LOG_WRN("Message queue full, dropping sample %u", sequence - 1);
        }

        k_msleep(SAMPLE_INTERVAL_MS);
    }
}

/* Logger/consumer thread */
static void logger_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    struct sensor_data data;

    LOG_INF("Logger thread started");

    while (1) {
        /* Block until data available */
        int ret = k_msgq_get(&sensor_msgq, &data, K_FOREVER);
        if (ret == 0) {
            LOG_INF("[%u] seq=%u temp=%d.%03d C, humid=%d.%02d %%",
                    data.timestamp,
                    data.sequence,
                    data.temperature / 1000,
                    data.temperature % 1000,
                    data.humidity / 1000,
                    (data.humidity % 1000) / 10);
        }
    }
}

int main(void)
{
    LOG_INF("Zephyr Data Logger - Starting");
    LOG_INF("Board: %s", CONFIG_BOARD);

    /* Create sensor thread */
    k_thread_create(&sensor_thread_data, sensor_stack,
                    K_THREAD_STACK_SIZEOF(sensor_stack),
                    sensor_thread_entry,
                    NULL, NULL, NULL,
                    SENSOR_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(&sensor_thread_data, "sensor");

    /* Create logger thread */
    k_thread_create(&logger_thread_data, logger_stack,
                    K_THREAD_STACK_SIZEOF(logger_stack),
                    logger_thread_entry,
                    NULL, NULL, NULL,
                    LOGGER_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(&logger_thread_data, "logger");

    LOG_INF("All threads created");

    return 0;
}
