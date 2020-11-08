SELECT DISTINCT name
FROM CatchedPokemon AS cp INNER JOIN Pokemon AS p ON cp.pid=p.id
WHERE level>=30 ORDER BY p.name;