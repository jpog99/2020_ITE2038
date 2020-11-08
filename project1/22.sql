SELECT type,count(type)
FROM Pokemon GROUP BY type 
ORDER BY count(type) ASC, type;