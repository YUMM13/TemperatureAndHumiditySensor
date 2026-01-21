SELECT
    "temperature",
    "humidity",
    CAST(
        parse_datetime(
            concat(
                cast("partition_0" AS varchar),
                cast("partition_1" AS varchar),
                cast("partition_2" AS varchar),
                lpad(cast("partition_3" AS varchar), 2, '0'),
                '00'
            ),
            'yyyyMMddHHmm'
        ) AS timestamp
    ) AS event_timestamp
FROM "temp_and_humidity_storage"
WHERE "status" = 'ok';
