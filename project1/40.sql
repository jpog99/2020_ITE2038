SELECT t1.hometown,nickname
FROM (SELECT hometown,level,nickname
      FROM CatchedPokemon, Trainer t
      WHERE owner_id=t.id) AS t1
      
      INNER JOIN 
      
      (SELECT hometown, MAX(level) AS max_level
        FROM CatchedPokemon, Trainer t
        WHERE owner_id=t.id
        GROUP BY T.hometown) AS t2
        
        ON t1.hometown=t2.hometown AND t1.level=t2.max_level;
        

