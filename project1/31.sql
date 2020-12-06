SELECT type
FROM Pokemon,Evolution
WHERE before_id=id
GROUP BY type HAVING count(type)>=3
ORDER BY type DESC;
