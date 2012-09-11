<?php
// (c)2012 Rackspace Hosting
// See COPYING for licensing information

namespace OpenCloud\DbService;

/**
 * This class represents a Database in the Rackspace "Red Dwarf"
 * database-as-a-service product.
 *
 * @author Glen Campbell <glen.campbell@rackspace.com>
 */
class Database extends \OpenCloud\Base {

    public
        $name;

    private
        $_instance;

    /**
     * Creates a new database object
     *
     * Unlike other objects (Servers, DataObjects, etc.), passing a database
     * name to the constructor does *not* pull information from the database.
     * For example, if you pass an ID to the `Server()` constructor, it will
     * attempt to retrieve the information on that server from the service,
     * and will return an error if it is not found. However, the Cloud
     * Databases service does not permit retrieval of information on
     * individual databases (only via Collection), and thus passing in a
     * name via the `$info` parameter only creates an in-memory object that
     * is not necessarily tied to an actual database.
     *
     * @param Instance $instance the parent DbService\Instance of the database
     * @param mixed $info if an array or object, treated as properties to set;
     *      if a string, treated as the database name
     * @return void
     * @throws DatabaseNameError if `$info` is not a string, object, or array
     */
    public function __construct(Instance $instance, $info=NULL) {
        $this->_instance = $instance;
        if (is_object($info) || is_array($info))
            foreach($info as $property => $value)
                $this->$property = $value;
        elseif (is_string($info))
            $this->name = $info;
        elseif (isset($info))
            throw new DatabaseNameError(
                _('Database parameter must be an object, array, or string'));
    }

    /**
     * returns the Url of the Database
     */
    public function Url() {
        if (!isset($this->name))
            throw new DatabaseNameError(
                _('The database does not have a Url yet'));
        return stripslashes($this->Instance()->Url('databases'))
                . '/' .$this->name;
    }

    /**
     * returns the Instance of the database
     */
    public function Instance() {
        return $this->_instance;
    }

    /**
     * returns the related service
     */
    public function Service() {
    	return $this->Instance()->Service();
    }

    /**
     * creates a new database
     *
     * @param
     * @return \OpenCloud\HttpResponse
     */
    public function Create($params=array()) {
        // target the /databases subresource
        $url = $this->Instance()->Url('databases');
        if (isset($params['name']))
        	$this->name = $params['name'];
        $json = json_encode($this->CreateJson($params));
        if ($this->CheckJsonError())
        	return FALSE;

        // POST it off
        $response = $this->Service()->Request(
        	$url,
        	'POST',
        	array(),
        	$json
        );

        // check the response code
        if ($response->HttpStatus() != 202)
        	throw new DatabaseCreateError(sprintf(
        		_('Error creating database [%s], status [%d] response [%s]'),
        		$this->name, $response->HttpStatus(), $response->HttpBody()));

        // refresh and return
        return $response;
    }

    /**
     * updates an existing database
     *
     * @throws DatabaseUpdateError always; updates are not permitted
     * @return void
     */
    public function Update($params=array()) {
    	throw new DatabaseUpdateError(
    		_('Updates are not currently permitted on Database objects'));
    }

    /**
     * deletes a database
     */
    public function Delete() {
    	$resp = $this->Service()->Request($this->Url(), 'DELETE');
    	if ($resp->HttpStatus() != 202)
    		throw new DatabaseDeleteError(sprintf(
    			_('Error deleting database [%s], status [%d] response [%s]'),
    			$this->name,
    			$resp->HttpStatus(),
    			$resp->HttpBody()));
    	return $resp;
    }

    /********** PRIVATE METHODS **********/

    /**
     * returns the JSON object for creating the database
     */
    private function CreateJson($params=array()) {
        $obj = new \stdClass();
        $obj->databases = array();
	    $obj->databases[0] = new \stdClass();

        // set the name
	    if (!isset($this->name))
	        throw new DatabaseNameError(
	            _('Database name is required'));
	    $obj->databases[0]->name = $this->name;
	    foreach($params as $key => $value)
	    	$obj->databases[0]->$key = $value;

        return $obj;
    }

} // class Database