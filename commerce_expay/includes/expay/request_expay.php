<?php
namespace expay;
//error_reporting(E_ALL);

/**
 * Class request_expay
 * This Class provides to connect to ExPay merchant`s service.
 * @version 150402
 * @author Vega
 */

require 'ExpayException.php';

/**
 * Class my_expay
 * @package expay
 */
class my_expay extends request_expay
{

    /**
     * @var request_expay
     */
    public $call_class;

    /**
     * @param $host
     * @param $kkey
     * @param $skey
     */
    public function __construct($host, $kkey, $skey)
    {
        try
        {

            //parent::__construct($host, $kkey, $skey);

            $this->call_class = new request_expay($host,$kkey,$skey);
        }
        catch (ExpayException $e)
        {
            // handle exception
        }

    }



}

/**
 * Class request_expay
 */
class request_expay
{

    /**
     * @var string
     */
    static $error_file = '!_errors_log.txt';

    public $debug = FALSE;
    /**
     * @var array
     */
    static $err_a=array();
    /**
     * @var string
     */
    static $status;
    /**
     *
     * @var string
     */
    static $message;

    /**
     * @var string
     */
    static $host;
    /**
     * @var
     */
    static $kkey;
    /**
     * @var
     */
    static $skey;


    /**
     * Getting variables for create class object
     * Constructor with arguments
     * @param $host
     * @param $kkey
     * @param $skey
     */
    public function __construct($host, $kkey, $skey) {

        if ( (!empty($host)) && (!empty($kkey)) && (!empty($skey)) )
        {
            switch($host)
            {
                case  'sandbox' :
                    self::$host ='https://api.sandbox.expay.asia/merchant/';
                    break;
                case  'prod' :
                    self::$host ='https://api.expay.asia/merchant/';
                    break;
                default:
                    self::$host ='Error Host';
                    throw new ExpayException('Error: Error Host!',500);
                    return null;

            }

            self::$kkey = $kkey;
            self::$skey = $skey;

        }
        else
        {
            throw new ExpayException('Error: One of variables is empty!',401);
        }


    }



    /**
     * Error Logging Function for show any call erorrs to merchant
     * @param $num_err - Code of Error
     * @param $str_err - Description of Error
     */
    private function _errors_log($num_err,$str_err)
    {
        if ($this->debug) {
            $f_err = @fopen(self::$error_file, 'a+');
            fputs($f_err, PHP_EOL . '/*** New Log ***/' . PHP_EOL . 'Date: ' . date('Y-m-d H:i:s') .' ['.time().']'. PHP_EOL);
            fputs($f_err, 'code: '.$num_err.' message:'.$str_err . PHP_EOL);
            fclose($f_err);
        }

        self::$err_a['error']= array('code'=>$num_err,'message'=>$str_err,'timestamp'=>time()); //self::$err_a;
    }


    /**
     * Response from merchant to Expay (Expay POST Request to merchant).
     * @param $init_arr - Associative Array from merchant`s data (described in Merchant`s Api Guide)
     * @return string
     */
    private function response_expay($init_arr)
    {
        $out=false;
        $time=time();
        if ( (is_array($init_arr)) && (array_key_exists('status', $init_arr)) && (array_key_exists('message', $init_arr)) )
        {
            $tmp_a=$init_arr;
            $tmp_a['timestamp']=$time;

            $str=json_encode($tmp_a);
            //$str=http_build_query($tmp_a,'','&');
            $this->_errors_log(__line__, __FUNCTION__ . ': $str=http_build_query : '.var_export($str,true));

            $hash=hash_hmac('sha1', $str, self::$skey);

            switch($tmp_a['status'])
            {

                case  '401' :
                    //case  '404' :
                case  '500' :
                    $s_tmp='error';
                    break;
                default:
                    $s_tmp='response';
            }

            $tmp_a=array($s_tmp=>$tmp_a,'hash'=>$hash);

            $out=json_encode($tmp_a);

        }
        else
        {
            $tmp_a=array();
            $tmp_a['status']='500';
            $tmp_a['message']='Error array data';
            $tmp_a['timestamp']=$time;
            $str=json_encode($tmp_a);
            //$str=http_build_query($tmp_a,'','&');
            $hash=hash_hmac('sha1', $str, self::$skey);

            $tmp_a=array('response'=>$tmp_a,'hash'=>$hash);
            $out=json_encode($tmp_a);
        }


        return $out;
    }

    /**
     * Parse test Post. For Test Post incoming to merchant from Expay.
     * Getting Associative Post Array ($_POST[]) and parse
     * @param $init_arr - $_POST[]
     * @return string - Name of method
     *
     */
    private function parse_post($init_arr)
    {
        $out=false;
        if (!empty($init_arr))
        {
            //$this->_errors_log(__line__, __FUNCTION__ . ': Data from Expay : '.var_export($init_arr,true));
            $method=$init_arr['method'];
            $old_hash=$init_arr['hash'];
            $str=http_build_query($init_arr,'','&');
            $responsea=explode('&hash=', $str);
            $new_hash=hash_hmac('sha1', $responsea[0], self::$skey);
            $this->_errors_log(__line__, __FUNCTION__ . ': $new_hash : '.var_export($new_hash,true));
            if (strcasecmp($old_hash, $new_hash) == 0)
            {
                $out=$method;
            }
            else
            {
                throw new ExpayException(__FUNCTION__.': Invalid Hash',401);
            }
        }

        else
        {
            throw new ExpayException(__FUNCTION__.': No data, empty Array',500);
        }
        return $out;
    }


    /**
     *
     *
     *
     */
    private  function _prep_arr($init_arr)
    {
        $prep_arr =  array_map("htmlentities",  array_map("strip_tags", $init_arr));
        return $prep_arr;
    }


    /**
     * Prepare string (build and sign) for send request to ExPay
     * @param $method - string name of merchant`s method
     * @param $init_arr - incoming Associative array with merchant`s variables (described in Merchant`s Api Guide)
     * @return string - prepare http build string with hash
     */
    private  function _prepare_str($method,$init_arr)
    {
        $time=time();

        //$init_arr=$this->_prep_arr($init_arr);
        $str=http_build_query($init_arr,'','&', PHP_QUERY_RFC3986);
        if ((strlen($str))>0)
        {$str.='&';}
        $str.='key='.self::$kkey.'&timestamp='.$time;
        $hash=hash_hmac('sha1', $method.'?'.$str, self::$skey);
        $request_str=$str."&hash=".$hash;
$this->_errors_log(401, __FUNCTION__ . ': '.$request_str);
        return $request_str;
    }


    /**
     * Check hash return from expay
     * @param $str
     * @return int
     * @throws ExpayException
     */
    private  function _check_hash($str)
    {
        $out=false;
        if ($str)
        {
            $json = json_decode($str);
            $json_arr = json_decode($str,true);

            if (isset($json_arr['response']))
            {
                $tmp_arr=explode('{"response":', $str);
                $tmp_arr=explode(',"hash":', $tmp_arr[1]);
                $old_hash='';
                if (isset($json->hash))
                {
                    $old_hash=$json->hash;
                }
                $new_hash=hash_hmac('sha1', $tmp_arr[0], self::$skey);
                if (strcasecmp($old_hash, $new_hash) == 0)
                {
                    $out=$json_arr['response'];
                }
                else
                {
                    throw new ExpayException(__FUNCTION__.': Invalid Hash Check',401);
                }
            }
            else
            {
                if (isset($json_arr['error']['code']) && isset($json_arr['error']['message']))
                {
                    throw new ExpayException($json_arr['error']['message'],$json_arr['error']['code']);
                }
            }
        }
        else
        {
            throw new ExpayException( __FUNCTION__ . ': No data for check HASH',404);
        }
        return $out;
    }
    /**
     * Send request to Expay
     * @function send_to_expay
     * @param $method - string name of merchant`s method
     * @param $str_send - http build string with hash
     * @return mixed - json response from Expay
     */
    private  function _send_to_expay($method,$str_send)
    {
        try {
            $out = false;
            $this->_errors_log(401, __FUNCTION__ . ': Try to connection curl to Expay, method: '.$method.', data: '.$str_send);
            //$f_err = @fopen(self::$error_file, 'a+');
            if ($curl = curl_init()) {
                curl_setopt($curl, CURLOPT_URL, self::$host . '/' . $method);
                curl_setopt($curl, CURLOPT_HEADER, false);
                curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
                curl_setopt($curl, CURLOPT_POST, true);
                curl_setopt($curl, CURLOPT_POSTFIELDS, $str_send);
                curl_setopt($curl, CURLOPT_CONNECTTIMEOUT, 60);
                curl_setopt($curl, CURLOPT_USERAGENT, 'ExPay SDK');

                //curl_setopt($curl, CURLOPT_VERBOSE, true);
                //curl_setopt($curl, CURLOPT_STDERR,  $f_err);

                curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, 1);
                curl_setopt($curl, CURLOPT_SSL_VERIFYHOST, 2);
                curl_setopt($curl, CURLOPT_CAINFO, realpath (dirname(__FILE__)).DIRECTORY_SEPARATOR.'expay.asia.crt'); //getcwd().DIRECTORY_SEPARATOR.'expay.asia.crt');

                $out = curl_exec($curl);
                $this->_errors_log(401, __FUNCTION__ . ': Try to connection curl to Expay, method: '.$method.', data: '.$out);
                $code= curl_getinfo ($curl, CURLINFO_HTTP_CODE);
                curl_close($curl);
                //fclose($f_err);

                if ( ($out) && ($code==200) )
                {
                    $out = $this->_check_hash($out);
                    //$this->_errors_log(401, __FUNCTION__ . ': line:.'.__line__.'  '.$method.', data: '.$out);
                }
                else
                {
                    throw new ExpayException( __FUNCTION__ . ': No data from Expay :'.$out,$code);
                }

            }
            return $out;
        }
        catch  (\Exception $e)
        {
            $out=false;
            return $out;
        }

    }


    /**
     * Call function from methods
     * @param $method - string name of method
     * @param $init_arr - incoming Associative array with merchant`s variables (described in Merchant`s Api Guide)
     * @return array|mixed
     */
    private  function _call_expay($method, $init_arr)
    {
        $out=false;
        if  (self::$host!='Error Host')
        {
            $request_str=$this->_prepare_str ($method,$init_arr);
            $out=$this->_send_to_expay($method,$request_str);
        }
        return $out;
    }


    /**
     * @return mixed
     */
    public function getinfoexpayerror()
    {
        $out=false;
        if (!@empty(self::$err_a['error']))
            $out=self::$err_a['error'];
        return $out;
    }

    /**
     * @param $name
     * @param $arguments
     * @return bool|mixed
     * @throws ExpayException
     */
    public function __call($name, $arguments)
    {
        $result = false;
        $prev_state = ini_set('display_errors', false);
        ob_start();
        $callee = array(get_class($this) , $name);
        if (method_exists(__CLASS__, $name))
        {      $methods=array('getMethods');
            if ( (in_array($name,$methods)) or ((!in_array($name,$methods)) && (!empty($arguments))) )
            {
                if (empty($arguments)) {$arguments=array();}
                try {
                    $result = call_user_func_array($callee, $arguments);
                } catch( ExpayException $e) {
                    $result = false;
                }
            }
            else
            {
                throw new ExpayException('Error: Don`t have some arguments!',401);
            }
        }
        else
        {
            throw new ExpayException('Error: Don`t have '.$name.' method!',404);
        }
        ob_end_clean();
        ini_set('display_errors', $prev_state);

        return $result;
    }
    /**
     * Return Associative Multi-Dimensional array  from response to ExPay merchant method getMethods
     * @return mixed - outcoming Associative array - response Expay (described in Merchant`s Api Guide)
     * and decoded json-to-array
     */
    private  function getMethods()
    {
        return $this->_call_expay(__FUNCTION__, array());
    }

    /**
     * Return Associative Multi-Dimensional array  from response to ExPay merchant method initPayment
     * @param $init_arr - incoming Associative array with merchant`s variables (described in Merchant`s Api Guide)
     * @return mixed - outcoming Associative array - response Expay (described in Merchant`s Api Guide)
     * and decoded json-to-array
     */
    private  function initPayment($init_arr)
    {
        return $this->_call_expay(__FUNCTION__, $init_arr);
    }


    /**
     * Return Associative Multi-Dimensional array  from response to ExPay merchant method getStatus
     * @param $init_arr - incoming Associative array with merchant`s variables (described in Merchant`s Api Guide)
     *
     * @return mixed - outcoming Associative array - response Expay (described in Merchant`s Api Guide)
     * and decoded json-to-array
     */
    private  function getStatus($init_arr)
    {
        return $this->_call_expay(__FUNCTION__, $init_arr);
    }

} // end class





?>
