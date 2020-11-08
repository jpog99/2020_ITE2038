SELECT DISTINCT name
FROM CatchedPokemon INNER JOIN Trainer AS t ON owner_id=t.id
WHERE level<=10 ORDER BY name;