SELECT name
FROM (SELECT count(type) as cnt,type
      FROM Pokemon
      GROUP BY type 
      ORDER BY count(type) DESC LIMIT 2) AS t1
      
INNER JOIN Pokemon p  
ON t1.type = p.type  
ORDER BY name    
