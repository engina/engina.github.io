using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.SQLite;
using System.Data.Common;
using FirebirdSql.Data.FirebirdClient;
using System.Collections;
using System.Data;
using System.Globalization;

namespace ConsoleApplication1
{
    class Program
    {
        static DbConnection _sqlite, _sqlce;
        static FbConnection _fbcon;

        static void Main(string[] args)
        {
            test(50);
            test(250);
            test(500);
            test(1000);
            test(5000);
            test(10000);
            test(20000);
            test(50000);
            System.Console.ReadKey();
        }

        static void test(int iter)
        {
            System.Console.WriteLine("\nTesting for " + iter + " iterations");
            DateTime t;
            TimeSpan elapsed;
            double i = iter;
            int rate = 0;
            // Init all
            SqlCeInit();
            SQLiteInit();
            FirebirdInit();
            //
            // SQLITE
            // 
            System.Console.Write("Testing sqlite... ");
            t = DateTime.Now;
            SQLiteInsert(iter);
            elapsed = (DateTime.Now - t);
            rate = (int)(i * 1000.0 / elapsed.TotalMilliseconds);
            System.Console.WriteLine("\t\t" + elapsed.TotalMilliseconds + "ms \t" + rate + " inserts / sec");

            //
            // Firebird 2
            //
            System.Console.Write("Testing firebird... ");
            t = DateTime.Now;
            FirebirdInsert(iter);
            elapsed = (DateTime.Now - t);
            rate = (int)(i * 1000.0 / elapsed.TotalMilliseconds);
            System.Console.WriteLine("\t\t" + elapsed.TotalMilliseconds + "ms \t" + rate + " inserts / sec");

            //
            // SQL CE
            //
            System.Console.Write("Testing MS SQL CE 3.5... ");
            t = DateTime.Now;
            SqlCeInsert(iter);
            elapsed = (DateTime.Now - t);
            rate = (int)(i * 1000.0 / elapsed.TotalMilliseconds);
            System.Console.WriteLine("\t" + elapsed.TotalMilliseconds + "ms \t" + rate + " inserts / sec");
        }
        static void SQLiteInit()
        {
            DbProviderFactory fact = DbProviderFactories.GetFactory("System.Data.SQLite");
            _sqlite = fact.CreateConnection();
            _sqlite.ConnectionString = "data source=sqlite.db";
            _sqlite.Open();
            /*
            DbCommand c = _sqlite.CreateCommand();
            c.CommandText = "PRAGMA synchronous = off";
            c.ExecuteNonQuery();
             */
        }

        static void SQLiteInsert(int iter)
        {
            using (DbTransaction trans = _sqlite.BeginTransaction())
            {
                DbCommand cmd = _sqlite.CreateCommand();
                for (int i = 0; i < iter; i++)
                {
                    cmd.CommandText = "INSERT INTO Log VALUES ( 1,2,3,4,5, date('now') )";
                    cmd.ExecuteNonQuery();
                }
                trans.Commit();
            }
        }

        static void FirebirdInit()
        {
            _fbcon = new FbConnection("Database=DATABASE.FDB;ServerType=1;");
            _fbcon.Open();
        }

        static void FirebirdInsert(int iter)
        {
            using (FbTransaction trans = _fbcon.BeginTransaction())
            {
                using (FbCommand cmd = _fbcon.CreateCommand())
                {
                    for (int i = 0; i < iter; i++)
                    {
                        cmd.Transaction = trans;
                        cmd.CommandText = "INSERT INTO LOGS VALUES ( 1,2,3,4,5, CURRENT_TIMESTAMP )";
                        cmd.ExecuteNonQuery();
                    }
                    trans.Commit();
                }
            }
        }

        static void SqlCeInit()
        {
            DbProviderFactory fact = DbProviderFactories.GetFactory("System.Data.SqlServerCe.3.5");
            _sqlce = fact.CreateConnection();
            _sqlce.ConnectionString = "Data Source=sqlcedb.sdf";
            _sqlce.Open();
        }

        static void SqlCeInsert(int iter)
        {
            using (DbTransaction trans = _sqlce.BeginTransaction())
            {
                DbCommand cmd = _sqlce.CreateCommand();
                for (int i = 0; i < iter; i++)
                {
                    cmd.CommandText = "INSERT INTO Logs VALUES ( 1,2,3,4,5, '1947/08/15 03:33:20' )";
                    cmd.ExecuteNonQuery();
                }
                trans.Commit();
            }
        }
    }
}
