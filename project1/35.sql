SELECT name
FROM Pokemon
WHERE id IN (SELECT before_id FROM Evolution WHERE after_id<before_id)
ORDER BY name;