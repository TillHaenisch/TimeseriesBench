
CREATE TABLE sensordata
(
  uuid VARCHAR(36) NOT NULL,
  time_ms BIGINT NOT NULL,
  val_type BIGINT NOT NULL,
  sensor_id BIGINT NOT NULL,
  value FLOAT NOT NULL
);

PARTITION TABLE sensordata ON COLUMN uuid;

CREATE PROCEDURE insert_sd
AS
   INSERT INTO sensordata VALUES(?,?,?,?,?);

PARTITION PROCEDURE insert_sd ON TABLE sensordata COLUMN uuid PARAMETER 0;
