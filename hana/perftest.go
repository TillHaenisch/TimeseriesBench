package main

import "fmt"
import "strconv"
import "github.com/SAP/go-hdb/driver"
import "database/sql"
import "time"
import "log"
import "math/rand"

func main() {
	fmt.Printf("Starting.\n")

	db, err := sql.Open(driver.DriverName, "hdb://SYSTEM:Datacast15@127.0.0.1:30015")
	if err != nil {
	    log.Fatal(err)
	}
	defer db.Close()

	stmt, err := db.Prepare("bulk insert into \"SYSTEM\".\"SENSORDATA\" values (?,?,?,?,?)")
	if err != nil {
	    log.Fatal(err)
	}
	defer stmt.Close()

	nr_rows := 100000
	t0 := time.Now()
	for i := 0; i < nr_rows; i++ {
	    if _, err := stmt.Exec(strconv.Itoa(i),123123,i%4,i%32,float64(rand.Intn(100000))); err != nil {
		log.Fatal(err)
	    }
	}
	t1 := time.Now()
	delta := t1.Sub(t0)
	fmt.Printf("Inserts: %v\n", nr_rows)
	fmt.Printf("The Inserts took %v to run.\n", delta)
	fmt.Printf("Time per Insert: %vns \n", delta.Nanoseconds()  / int64(nr_rows))
	fmt.Printf("Inserts/s: %v \n", ( float64(nr_rows) / float64(delta.Nanoseconds())) * float64(1000000000) )
	fmt.Printf("finished \n")
	// Call final stmt.Exec().
	if _, err := stmt.Exec(); err != nil {
	    log.Fatal(err)
	}
}
