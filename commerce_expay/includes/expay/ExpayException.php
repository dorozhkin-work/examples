<?php
namespace expay;
/**
 * @version 150402
 * @author Vega
 * Date: 23.03.15
 * Time: 12:47
 */

class ExpayException extends \Exception {
    private static $error_msg, $error_code;

    public function __construct($error_msg,$error_code)
    {
        self::$error_msg=$error_msg;
        self::$error_code=$error_code;
        parent::__construct($error_msg, $error_code);
        Logger::expayLog($this);
    }


}

class Logger
{
    static $error_file = '!_errors_log.txt';

    public static function expayLog(ExpayException $exception, $error_file = '') {
        if (empty($error_file)) {
            $error_file = self::$error_file;
        }

        $message = $exception->getMessage();
        $code = $exception->getCode();
        $file = $exception->getFile();
        $line = $exception->getLine();
        $trace = $exception->getTraceAsString();
        $date = date('Y-m-d H:i:s');

        request_expay::$err_a['error']= array('code'=>$code,'message'=>$message,'timestamp'=>time());

        $log_message = "Exception:
            Date: {$date}
            Msg: {$message}
            Code: {$code}
            File: {$file}
            Line: {$line}";
        //Stack trace:{$trace}


        $f_err = @fopen($error_file, 'a+');
        fputs($f_err, PHP_EOL . '/*** New Error ***/' . PHP_EOL . '[Time]:' . time() . PHP_EOL);
        fputs($f_err,$log_message . PHP_EOL);
        fclose($f_err);
    }
}


set_exception_handler('expay\Logger::expayLog');


?>
